export Msg, Prefetch, GetImage, Exit;
export ImageResponseMsg, ImageReady, ImageNotReady;
export ImageCacheTask;
export image_cache_task;

import image::base::{Image, load_from_memory, test_image_bin};
import std::net::url::url;
import util::url::{make_url, UrlMap, url_map};
import comm::{chan, port};
import task::spawn_listener;
import resource::resource_task;
import resource_task::ResourceTask;
import core::arc::arc;
import clone_arc = core::arc::clone;

enum Msg {
    Prefetch(url),
    GetImage(url, chan<ImageResponseMsg>),
    Exit
}

enum ImageResponseMsg {
    ImageReady(arc<~Image>),
    ImageNotReady
}

type ImageCacheTask = chan<Msg>;

fn image_cache_task(resource_task: ResourceTask) -> ImageCacheTask {
    do spawn_listener |from_client| {
        ImageCache {
            resource_task: resource_task,
            from_client: from_client,
            prefetch_map: url_map(),
            image_map: url_map()
        }.run();
    }
}

struct ImageCache {
    resource_task: ResourceTask;
    from_client: port<Msg>;
    prefetch_map: UrlMap<@PrefetchData>;
    image_map: UrlMap<@arc<~Image>>;
}

struct PrefetchData {
    response_port: port<resource_task::ProgressMsg>;
    mut data: ~[u8];
}

impl ImageCache {

    fn run() {

        loop {
            match self.from_client.recv() {
              Prefetch(url) => self.prefetch(url),
              GetImage(url, response) => self.get_image(url, response),
              Exit => break
            }
        }
    }

    /*priv*/ fn prefetch(url: url) {
        if self.image_map.contains_key(url) {
            // We've already decoded this image
            return
        }

        if self.prefetch_map.contains_key(url) {
            // We're already waiting for this image
            return
        }
        let response_port = port();
        self.resource_task.send(resource_task::Load(url, response_port.chan()));

        let prefetch_data = @PrefetchData {
            response_port: response_port,
            data: ~[]
        };

        self.prefetch_map.insert(url, prefetch_data);
    }

    /*priv*/ fn get_image(url: url, response: chan<ImageResponseMsg>) {
        match self.image_map.find(url) {
          some(image) => {
            response.send(ImageReady(clone_arc(image)));
            return
          }
          none => ()
        }

        match self.prefetch_map.find(url) {
          some(prefetch_data) => {

            let mut image_sent = false;

            while prefetch_data.response_port.peek() {
                match prefetch_data.response_port.recv() {
                  resource_task::Payload(data) => {
                    prefetch_data.data += data;
                  }
                  resource_task::Done(result::ok(*)) => {
                    // We've got the entire image binary
                    let mut data = ~[];
                    data <-> prefetch_data.data;
                    // FIXME: Need to do this in parallel
                    let image = @arc(~load_from_memory(data));
                    response.send(ImageReady(clone_arc(image)));
                    self.prefetch_map.remove(url);
                    self.image_map.insert(url, image);
                    image_sent = true;
                    break;
                  }
                  resource_task::Done(result::err(*)) => {
                    // FIXME: need to actually report the failed image load
                    self.prefetch_map.remove(url);
                    break;
                  }
                }
            }

            if !image_sent {
                response.send(ImageNotReady);
            }
          }
          none => fail ~"got a request for image data without prefetch"
        }
    }
}

#[test]
fn should_exit_on_request() {

    let mock_resource_task = do spawn_listener |from_client| {

        // infer me
        let from_client: port<resource_task::ControlMsg> = from_client;

        loop {
            match from_client.recv() {
              resource_task::Exit => break,
              _ => ()
            }
        }
    };

    let image_cache_task = image_cache_task(mock_resource_task);
    let url = make_url(~"file", none);

    image_cache_task.send(Exit);
    mock_resource_task.send(resource_task::Exit);
}

#[test]
#[should_fail]
fn should_fail_if_unprefetched_image_is_requested() {

    let mock_resource_task = do spawn_listener |from_client| {
    };

    let image_cache_task = image_cache_task(mock_resource_task);
    let url = make_url(~"file", none);

    let request = port();
    image_cache_task.send(GetImage(url, request.chan()));
    request.recv();
}

#[test]
fn should_request_url_from_resource_task_on_prefetch() {
    let url_requested = port();
    let url_requested_chan = url_requested.chan();

    let mock_resource_task = do spawn_listener |from_client| {

        // infer me
        let from_client: port<resource_task::ControlMsg> = from_client;

        loop {
            match from_client.recv() {
              resource_task::Load(url, _) => url_requested_chan.send(()),
              resource_task::Exit => break
            }
        }
    };

    let image_cache_task = image_cache_task(mock_resource_task);
    let url = make_url(~"file", none);

    image_cache_task.send(Prefetch(url));
    url_requested.recv();
    image_cache_task.send(Exit);
    mock_resource_task.send(resource_task::Exit);
}

#[test]
fn should_not_request_url_from_resource_task_on_multiple_prefetches() {
    let url_requested = port();
    let url_requested_chan = url_requested.chan();

    let mock_resource_task = do spawn_listener |from_client| {

        // infer me
        let from_client: port<resource_task::ControlMsg> = from_client;

        loop {
            match from_client.recv() {
              resource_task::Load(url, _) => url_requested_chan.send(()),
              resource_task::Exit => break
            }
        }
    };

    let image_cache_task = image_cache_task(mock_resource_task);
    let url = make_url(~"file", none);

    image_cache_task.send(Prefetch(url));
    image_cache_task.send(Prefetch(url));
    url_requested.recv();
    image_cache_task.send(Exit);
    mock_resource_task.send(resource_task::Exit);
    assert !url_requested.peek()
}

#[test]
fn should_return_image_not_ready_if_data_has_not_arrived() {
    let mock_resource_task = do spawn_listener |from_client| {

        // infer me
        let from_client: port<resource_task::ControlMsg> = from_client;

        loop {
            match from_client.recv() {
              resource_task::Exit => break,
              _ => ()
            }
        }
    };

    let image_cache_task = image_cache_task(mock_resource_task);
    let url = make_url(~"file", none);

    image_cache_task.send(Prefetch(url));
    let response_port = port();
    image_cache_task.send(GetImage(url, response_port.chan()));
    assert response_port.recv() == ImageNotReady;
    image_cache_task.send(Exit);
    mock_resource_task.send(resource_task::Exit);
}

#[test]
fn should_return_decoded_image_data_if_data_has_arrived() {

    let image_bin_sent = port();
    let image_bin_sent_chan = image_bin_sent.chan();

    let mock_resource_task = do spawn_listener |from_client| {

        // infer me
        let from_client: port<resource_task::ControlMsg> = from_client;

        loop {
            match from_client.recv() {
              resource_task::Load(_, response) => {
                response.send(resource_task::Payload(test_image_bin()));
                response.send(resource_task::Done(result::ok(())));
                image_bin_sent_chan.send(());
              }
              resource_task::Exit => break
            }
        }
    };

    let image_cache_task = image_cache_task(mock_resource_task);
    let url = make_url(~"file", none);

    image_cache_task.send(Prefetch(url));

    // Wait until our mock resource task has sent the image to the image cache
    image_bin_sent.recv();

    let response_port = port();
    image_cache_task.send(GetImage(url, response_port.chan()));
    match response_port.recv() {
      ImageReady(_) => (),
      _ => fail
    }

    image_cache_task.send(Exit);
    mock_resource_task.send(resource_task::Exit);
}

#[test]
fn should_return_decoded_image_data_for_multiple_requests() {

    let image_bin_sent = port();
    let image_bin_sent_chan = image_bin_sent.chan();

    let mock_resource_task = do spawn_listener |from_client| {

        // infer me
        let from_client: port<resource_task::ControlMsg> = from_client;

        loop {
            match from_client.recv() {
              resource_task::Load(_, response) => {
                response.send(resource_task::Payload(test_image_bin()));
                response.send(resource_task::Done(result::ok(())));
                image_bin_sent_chan.send(());
              }
              resource_task::Exit => break
            }
        }
    };

    let image_cache_task = image_cache_task(mock_resource_task);
    let url = make_url(~"file", none);

    image_cache_task.send(Prefetch(url));

    // Wait until our mock resource task has sent the image to the image cache
    image_bin_sent.recv();

    for iter::repeat(2) {
        let response_port = port();
        image_cache_task.send(GetImage(url, response_port.chan()));
        match response_port.recv() {
          ImageReady(_) => (),
          _ => fail
        }
    }

    image_cache_task.send(Exit);
    mock_resource_task.send(resource_task::Exit);
}

#[test]
fn should_not_request_image_from_resource_task_if_image_is_already_available() {

    let image_bin_sent = port();
    let image_bin_sent_chan = image_bin_sent.chan();

    let resource_task_exited = port();
    let resource_task_exited_chan = resource_task_exited.chan();

    let mock_resource_task = do spawn_listener |from_client| {

        // infer me
        let from_client: port<resource_task::ControlMsg> = from_client;

        loop {
            match from_client.recv() {
              resource_task::Load(_, response) => {
                response.send(resource_task::Payload(test_image_bin()));
                response.send(resource_task::Done(result::ok(())));
                image_bin_sent_chan.send(());
              }
              resource_task::Exit => {
                resource_task_exited_chan.send(());
                break
              }
            }
        }
    };

    let image_cache_task = image_cache_task(mock_resource_task);
    let url = make_url(~"file", none);

    image_cache_task.send(Prefetch(url));

    // Wait until our mock resource task has sent the image to the image cache
    image_bin_sent.recv();

    let response_port = port();
    image_cache_task.send(GetImage(url, response_port.chan()));
    match response_port.recv() {
      ImageReady(_) => (),
      _ => fail
    }

    image_cache_task.send(Prefetch(url));

    let response_port = port();
    image_cache_task.send(GetImage(url, response_port.chan()));
    response_port.recv();

    image_cache_task.send(Exit);
    mock_resource_task.send(resource_task::Exit);

    resource_task_exited.recv();

    // Our resource task should not have received another request for the image
    // because it's already cached
    assert !image_bin_sent.peek();
}

#[test]
fn should_return_not_ready_if_image_bin_cannot_be_fetched() {

    let image_bin_sent = port();
    let image_bin_sent_chan = image_bin_sent.chan();

    let mock_resource_task = do spawn_listener |from_client| {

        // infer me
        let from_client: port<resource_task::ControlMsg> = from_client;

        loop {
            match from_client.recv() {
              resource_task::Load(_, response) => {
                response.send(resource_task::Payload(test_image_bin()));
                // ERROR fetching image
                response.send(resource_task::Done(result::err(())));
                image_bin_sent_chan.send(());
              }
              resource_task::Exit => break
            }
        }
    };

    let image_cache_task = image_cache_task(mock_resource_task);
    let url = make_url(~"file", none);

    image_cache_task.send(Prefetch(url));

    // Wait until our mock resource task has sent the image to the image cache
    image_bin_sent.recv();

    let response_port = port();
    image_cache_task.send(GetImage(url, response_port.chan()));
    match response_port.recv() {
      ImageNotReady => (),
      _ => fail
    }

    image_cache_task.send(Exit);
    mock_resource_task.send(resource_task::Exit);
}

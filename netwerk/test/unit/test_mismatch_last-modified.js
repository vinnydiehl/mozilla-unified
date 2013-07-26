const Cc = Components.classes;
const Ci = Components.interfaces;
const Cu = Components.utils;
const Cr = Components.results;

Cu.import("resource://testing-common/httpd.js");
var httpserver = new HttpServer();

var ios;

// Test the handling of a cache revalidation with mismatching last-modified
// headers. If we get such a revalidation the cache entry should be purged.
// see bug 717350

// In this test the wrong data is from 11-16-1994 with a value of 'A',
// and the right data is from 11-15-1994 with a value of 'B'.

// the same URL is requested 3 times. the first time the wrong data comes
// back, the second time that wrong data is revalidated with a 304 but
// a L-M header of the right data (this triggers a cache purge), and
// the third time the right data is returned.

var listener_3 = {
    // this listener is used to process the the request made after
    // the cache invalidation. it expects to see the 'right data'

    QueryInterface: function(iid) {
	if (iid.equals(Components.interfaces.nsIStreamListener) ||
            iid.equals(Components.interfaces.nsIRequestObserver) ||
            iid.equals(Components.interfaces.nsISupports))
	    return this;
	throw Components.results.NS_ERROR_NO_INTERFACE;
    },

    onStartRequest: function test_onStartR(request, ctx) {},
    
    onDataAvailable: function test_ODA(request, cx, inputStream,
                                       offset, count) {
	var data = new BinaryInputStream(inputStream).readByteArray(count);
      
	do_check_eq(data[0], "B".charCodeAt(0));
    },

    onStopRequest: function test_onStopR(request, ctx, status) {
	httpserver.stop(do_test_finished);
    }
};

XPCOMUtils.defineLazyGetter(this, "listener_2", function() {
    return {
    // this listener is used to process the revalidation of the
    // corrupted cache entry. its revalidation prompts it to be cleaned

    QueryInterface: function(iid) {
	if (iid.equals(Components.interfaces.nsIStreamListener) ||
            iid.equals(Components.interfaces.nsIRequestObserver) ||
            iid.equals(Components.interfaces.nsISupports))
	    return this;
	throw Components.results.NS_ERROR_NO_INTERFACE;
    },

    onStartRequest: function test_onStartR(request, ctx) {},
    
    onDataAvailable: function test_ODA(request, cx, inputStream,
                                       offset, count) {
	var data = new BinaryInputStream(inputStream).readByteArray(count);
      
	// This is 'A' from a cache revalidation, but that reval will clean the cache
	// because of mismatched last-modified response headers
	
	do_check_eq(data[0], "A".charCodeAt(0));
    },

    onStopRequest: function test_onStopR(request, ctx, status) {
	var channel = request.QueryInterface(Ci.nsIHttpChannel);

	var chan = ios.newChannel("http://localhost:" +
				  httpserver.identity.primaryPort +
				  "/test1", "", null);
	chan.asyncOpen(listener_3, null);
    }
};
});

XPCOMUtils.defineLazyGetter(this, "listener_1", function() {
    return {
    // this listener processes the initial request from a empty cache.
    // the server responds with the wrong data ('A')

    QueryInterface: function(iid) {
	if (iid.equals(Components.interfaces.nsIStreamListener) ||
            iid.equals(Components.interfaces.nsIRequestObserver) ||
            iid.equals(Components.interfaces.nsISupports))
	    return this;
	throw Components.results.NS_ERROR_NO_INTERFACE;
    },

    onStartRequest: function test_onStartR(request, ctx) {},
    
    onDataAvailable: function test_ODA(request, cx, inputStream,
                                       offset, count) {
	var data = new BinaryInputStream(inputStream).readByteArray(count);
	do_check_eq(data[0], "A".charCodeAt(0));
    },

    onStopRequest: function test_onStopR(request, ctx, status) {
	var channel = request.QueryInterface(Ci.nsIHttpChannel);

	var chan = ios.newChannel("http://localhost:" +
				  httpserver.identity.primaryPort +
				  "/test1", "", null);
	chan.asyncOpen(listener_2, null);
    }
};
});

function run_test() {
    do_get_profile();
    ios = Cc["@mozilla.org/network/io-service;1"]
            .getService(Ci.nsIIOService);

    evict_cache_entries();

    httpserver.registerPathHandler("/test1", handler);
    httpserver.start(-1);

    var port = httpserver.identity.primaryPort;

    var chan = ios.newChannel("http://localhost:" + port + "/test1", "", null);
    chan.asyncOpen(listener_1, null);

    do_test_pending();
}

var iter=0;
function handler(metadata, response) {
    iter++;
    if (metadata.hasHeader("If-Modified-Since")) {
	response.setStatusLine(metadata.httpVersion, 304, "Not Modified");
	response.setHeader("Last-Modified", "Tue, 15 Nov 1994 12:45:26 GMT", false);
    }    
    else {
	response.setStatusLine(metadata.httpVersion, 200, "OK");
	response.setHeader("Cache-Control", "max-age=0", false)
	if (iter == 1) {
	    // simulated wrong response
	    response.setHeader("Last-Modified", "Wed, 16 Nov 1994 00:00:00 GMT", false);
	    response.bodyOutputStream.write("A", 1);
	}
	if (iter == 3) {
	    // 'correct' response
	    response.setHeader("Last-Modified", "Tue, 15 Nov 1994 12:45:26 GMT", false);
	    response.bodyOutputStream.write("B", 1);
	}
    }
}

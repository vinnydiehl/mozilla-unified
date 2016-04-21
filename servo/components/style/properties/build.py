# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import json
import os.path
import sys

BASE = os.path.dirname(__file__)
sys.path.insert(0, os.path.join(BASE, "Mako-0.9.1.zip"))

from mako import exceptions
from mako.lookup import TemplateLookup
from mako.template import Template

import data


def main():
    usage = "Usage: %s [ servo | gecko ] [ style-crate | geckolib | html ]" % sys.argv[0]
    if len(sys.argv) < 3:
        abort(usage)
    product = sys.argv[1]
    output = sys.argv[2]
    if product not in ["servo", "gecko"] or output not in ["style-crate", "geckolib", "html"]:
        abort(usage)

    properties = data.PropertiesData(product=product)
    rust = render(os.path.join(BASE, "properties.mako.rs"), product=product, data=properties)
    if output == "style-crate":
        write(os.environ["OUT_DIR"], "properties.rs", rust)
    if output == "geckolib":
        template = os.path.join(BASE, "..", "..", "..", "ports", "geckolib", "properties.mako.rs")
        rust = render(template, data=properties)
        write(os.environ["OUT_DIR"], "properties.rs", rust)
    elif output == "html":
        write_html(properties)


def abort(message):
    sys.stderr.write(message + b"\n")
    sys.exit(1)


def render(filename, **context):
    try:
        lookup = TemplateLookup(directories=[BASE])
        template = Template(open(filename, "rb").read(),
                            filename=filename,
                            input_encoding="utf8",
                            lookup=lookup,
                            strict_undefined=True)
        # Uncomment to debug generated Python code:
        # write("/tmp", "mako_%s.py" % os.path.basename(filename), template.code)
        return template.render(**context).encode("utf8")
    except:
        # Uncomment to see a traceback in generated Python code:
        # raise
        abort(exceptions.text_error_template().render().encode("utf8"))


def write(directory, filename, content):
    if not os.path.exists(directory):
        os.makedirs(directory)
    open(os.path.join(directory, filename), "wb").write(content)


def write_html(properties):
    properties = dict(
        (p.name, {
            "flag": p.experimental,
            "shorthand": hasattr(p, "sub_properties")
        })
        for p in properties.longhands + properties.shorthands
    )
    doc_servo = os.path.join(BASE, "..", "..", "..", "target", "doc", "servo")
    html = render(os.path.join(BASE, "properties.html.mako"), properties=properties)
    write(doc_servo, "css-properties.html", html)
    write(doc_servo, "css-properties.json", json.dumps(properties, indent=4))


if __name__ == "__main__":
    main()


# osc-receiver

[![Build Status](https://travis-ci.org/CircuitLab/osc-receiver.svg?branch=master)](https://travis-ci.org/CircuitLab/osc-receiver)

A tiny OSC message receiver.

## Installation

npm:

    $ npm install osc-receiver

## Usage

Example:

```js
var OscReceiver = require('osc-receiver')
  , receiver = new OscReceiver();

receiver.bind(9337);

receiver.on('/foo', function(a, b, c) {
  // do something.
});

receiver.on('/bar', function(x, y) {
  // do something.
});

receiver.on('message', function() {
  // handle all messages
  var address = arguments[0];
  var args = Array.prototype.slice.call(arguments, 1);
});
```

## License

The MIT License

Copyright (c) 2013 Circuit Lab. &lt;info@uniba.jp&gt;

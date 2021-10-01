
/**
 * Module dependencies.
 */

var debug = require('debug')('osc-receiver');
var dgram = require('dgram');
var osc = require('osc-min');
var EventEmitter = require('events').EventEmitter;

/**
 * Expose `OscReceiver`.
 */

module.exports = OscReceiver;

/**
 * OscReceiver constructor.
 */

function OscReceiver() {
  this._socket = dgram.createSocket('udp4');
  this._socket.on('message', this._onMessage.bind(this));
}

/**
 * Inherits from `EventEmitter`.
 */

OscReceiver.prototype.__proto__ = EventEmitter.prototype;

/**
 * @param {Number} port
 * @param {String} address
 * @param {Function} callback
 */

OscReceiver.prototype.bind = function(port, address, callback) {
  this._socket.bind.apply(this._socket, arguments);
  return this;
};

/**
 * @param {Object} msg
 * @param {Object} rInfo
 */

OscReceiver.prototype._onMessage = function(msg, rInfo) {
  try {
    var message = osc.fromBuffer(msg);
  } catch (e) {
    return this.emit('error', e);
  }

  var self = this;
  var elements = 'bundle' === message.oscType ? message.elements : [message];

  elements.forEach(function(el, index) {
    var args = [el.address];
    el.args.forEach(function(arg, index) {
      args.push(arg.value);
    });
    debug('receive %j from %s:%s', args, rInfo.address, rInfo.port);
    self.emit.apply(self, ['message'].concat(args));
    self.emit.apply(self, args);
  });

  return this;
};

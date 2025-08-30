#!/usr/bin/env -S node -i -r

// A series of utilities for convinience usage
// Usage:
//   ./repl.js

String.prototype.toStr = function() {
    // Convert the escaped string (which is interpreted as Latin1 bytes) to Uint8Array
    const bytes = new Uint8Array(this.length);
    for (let i = 0; i < this.length; i++) {
        bytes[i] = this.charCodeAt(i);
    }
    const decoder = new TextDecoder('utf-8');
    return decoder.decode(bytes);
};

String.prototype.toUTF32Array = function() { return this.split("").map(c => c.charCodeAt(0)); };

String.prototype.toHex = function() { return this.toUTF32Array().map(c => `\\x${c.toString(16)}`).join(''); };

String.prototype.isNFC = function() { return this.normalize("NFC") == this; };

String.prototype.isNFD = function() { return this.normalize("NFD") == this; };

String.prototype.toNFC = function() { return this.normalize("NFC"); };

String.prototype.toNFD = function() { return this.normalize("NFD"); };

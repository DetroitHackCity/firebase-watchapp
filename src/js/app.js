
require('firebase_lib');

var Clay = require('clay');
var clayConfig = require('config.json');
var clay = new Clay(clayConfig);

function firebaseCall(username, steps) {
	var myDataRef = new Firebase('https://nk6bl51k7ig.firebaseio-demo.com/');
	myDataRef.push({name: username, text: steps});
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
	function(e) {
		console.log('PebbleKit JS ready!');
	}
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
	function(e) {
		console.log('AppMessage received!');

		var dict = e.payload;
		console.log('Got message: ' + JSON.stringify(dict));

		firebaseCall(e.payload.username, e.payload.steps);
	}                     
);

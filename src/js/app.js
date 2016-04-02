
require('firebase_lib');

function firebaseCall(value) {
	var myDataRef = new Firebase('https://nk6bl51k7ig.firebaseio-demo.com/');
	myDataRef.push({name: value, text: value});
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

		firebaseCall(e.payload.dir);
	}                     
);
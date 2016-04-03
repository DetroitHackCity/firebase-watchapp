
require('firebase_lib');

var Clay = require('clay');
var clayConfig = require('config.json');
var clay = new Clay(clayConfig);

function firebaseCall(username, steps) {
	var myDataRef = new Firebase('https://nk6bl51k7ig.firebaseio-demo.com/');
	myDataRef.push({name: username, text: steps});
}

function firebaseRead() {

/*
	var ref = new Firebase("https://nk6bl51k7ig.firebaseio-demo.com");
	// Attach an asynchronous callback to read the data
	ref.on("value", function(snapshot) {
	  console.log('snapshot.val(): ' + JSON.stringify(snapshot.val()));
	}, function (errorObject) {
	  console.log("The read failed: " + errorObject.code);
	});
*/

	// TODO: FIGURE OUT HOW TO NOT MAKE THIS SUPER INEFFICIENT!!
	var leaderName = "";
	var leaderSteps = 0;

	var ref = new Firebase("https://nk6bl51k7ig.firebaseio-demo.com");
	ref.orderByChild("text").on("child_added", function(snapshot) {
	  //console.log("name: " + snapshot.key() + " steps: " + snapshot.val().text);
	  leaderName = snapshot.val().name;
	  leaderSteps = snapshot.val().text;

	  console.log("leaderName: " + leaderName + " leaderSteps: " + leaderSteps);
	});

	// TODO: pass results
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

		//firebaseCall(e.payload.username, e.payload.steps);
		firebaseRead();
	}                     
);

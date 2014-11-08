var app = require('express')();
var web_server = require('http').Server(app);
var io = require('socket.io')(web_server);
var net = require('net');

var tcp_port = 8124, web_port = 8090, sonar_data = {};

// Start web server on this port
web_server.listen(web_port);

// send clients to index page
app.get('/', function (req, res) {
	res.sendFile(__dirname + '/web/index.html');
}).get('/index.html', function (req, res) {
	res.sendFile(__dirname + '/web/index.html');
}).get('/d3.v3.min.js', function (req, res) {
	res.sendFile(__dirname + '/web/d3.v3.min.js');
}).get('/jquery.min.js', function (req, res) {
	res.sendFile(__dirname + '/web/jquery.min.js');
}).get('/jquery.jsonview.js', function (req, res) {
	res.sendFile(__dirname + '/web/jquery.jsonview.js');
}).get('/jquery.jsonview.css', function (req, res) {
	res.sendFile(__dirname + '/web/jquery.jsonview.css');
}).get('/jquery.ui.min.js', function (req, res) {
	res.sendFile(__dirname + '/web/jquery.ui.min.js');
});

// on new IO connections
io.on('connection', function (socket) {
	// Emit existing sonar data
	socket.emit('old sonar', sonar_data);

	socket.on('reset data', function(){
		resetSonarData();
	});
});

var updateSonarData = function(data) {
	// if new sonar
	if(!sonar_data[data.i]){
		sonar_data[data.i] = {};
	}

	// only update what has changed
	if(data.h){
		sonar_data[data.i].h = data.h;
	}

	if(data.d){
		sonar_data[data.i].d = data.d;
	}

};

var resetSonarData = function(){
	sonar_data = {};
};

// Create simple TCP server
var tcp_server = net.createServer(function(c) {
	console.log('TCP client connected');
	c.on('end', function() {
		console.log('TCP client disconnected');
	});

	c.on('data', function(data) {
		var recvData = data.toString().trim();
		
		console.log("Recv data: " + recvData);

		try {
			var parsedObj = JSON.parse(recvData);

			// Add to sonar data array
			updateSonarData(parsedObj);

			// emit to all connected sockets io clients
			io.emit('new sonar', parsedObj);
		} catch (e) {
			console.error("Could not parse JSON: " + recvData);
			console.error(e);
		}
	});
});
tcp_server.listen(tcp_port, function() {
	console.log('TCP server bound to port ' + tcp_port);
});
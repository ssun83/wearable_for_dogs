const serviceUuid = "19b10010-e8f2-537e-4f6c-d104768a1214";
let myCharacteristic;
let myValue = "0";
let myBLE;
let isConnected = false;

// variables del html
let connectButton = document.getElementById("connect");
let disconnectButton = document.getElementById("disconnect");
let repetitionSelect = document.getElementById("repetitions");
let startButton = document.getElementById("play");

myBLE = new p5ble();

repetitionSelect.addEventListener('change', function () {
  document.getElementById("rep_label").innerHTML = "0/" + document.getElementById("repetitions").value.toString();
})

connectButton.addEventListener('click', function () {
  connectToBle();
})

disconnectButton.addEventListener('click', function () {
  disconnectToBle();
})

startButton.addEventListener('click', function () {
  readToBle();
})

function connectToBle() {
  document.getElementById("wearable_status").innerHTML = "Wearable connected!";

  // Connect to a device by passing the service UUID
  myBLE.connect(serviceUuid, gotCharacteristics);
}

function disconnectToBle() {
  document.getElementById("wearable_status").innerHTML = "Wearable disconnected!";

  // Disonnect to the device
  myBLE.disconnect();
  // Check if myBLE is connected
  isConnected = myBLE.isConnected();
}

function onDisconnected() {
  console.log('Device got disconnected.');
  isConnected = false;
}


// A function that will be called once got characteristics
function gotCharacteristics(error, characteristics) {

  if (error) console.log('error: ', error);
  console.log('characteristics: ', characteristics);
  myCharacteristic = characteristics[0];
  isConnected = myBLE.isConnected();
}

function readToBle() {
  myBLE.read(myCharacteristic, gotValue);
}

// A function that will be called once got values
function gotValue(error, value) {

  if (error) console.log('error: ', error);

  console.log('value: ', value);
  myValue = value;

  // Add a event handler when the device is disconnected
  myBLE.onDisconnected(onDisconnected)

  if (isConnected) {
    // After getting a value, call p5ble.read() again to get the value again
    // myBLE.read(myCharacteristic, gotValue);
    // You can also pass in the dataType
    // Options: 'unit8', 'uint16', 'uint32', 'int8', 'int16', 'int32', 'float32', 'float64', 'string'
    myBLE.read(myCharacteristic, 'string', gotValue);
    document.getElementById("angle").innerHTML = myValue;


  }
}

let minutes = '00',
  seconds = '00',
  chronometerDisplay = document.querySelector('[data-chronometer]'),
  chronometerCall

function chronometer() {

  seconds++

  if (seconds < 10) seconds = '0' + seconds

  if (seconds > 59) {
    seconds = '00'
    minutes++

    if (minutes < 10) minutes = '0' + minutes
  }

  if (minutes > 59) {
    minutes = '00'
  }

  chronometerDisplay.textContent = `${minutes}:${seconds}`

}

play.onclick = (event) => {
  chronometerCall = setInterval(chronometer, 1000)
  event.target.setAttribute(`disabled`, ``)
}

pause.onclick = () => {
  clearInterval(chronometerCall)
  play.removeAttribute(`disabled`)
}
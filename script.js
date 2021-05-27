const serviceUuid = "19b10010-e8f2-537e-4f6c-d104768a1214";
let myCharacteristic;
let myValue = "0";
let myBLE;
let isConnected = false;

// values
let minAngleValue = "0";
let maxAngleValue = "0";
let minReachedValue = "180";
let maxReachedValue = "0";

// buttons
let connectButton = document.getElementById("connectButton");
let disconnectButton = document.getElementById("disconnectButton");
let startButton = document.getElementById("playButton");
let setButton = document.getElementById("setButton");
let mailButton = document.getElementById("mailButton");

// selects
let repSelect = document.getElementById("repSelect");
let breedSelect = document.getElementById("breedSelect");
let jointSelect = document.getElementById("jointSelect");

// labels
let repLabel = document.getElementById("repLabel");
let safeRangeLabel = document.getElementById("safeRangeLabel")
let wearableStatus = document.getElementById("wearableStatus");
let angleLabel = document.getElementById("angleROMLabel");
let minReachedLabel = document.getElementById("minReachedLabel");
let maxReachedLabel = document.getElementById("maxReachedLabel");

// audio
var max_angle_audio = new Audio('full_extension.mp3');
var min_angle_audio = new Audio('full_flexion.mp3');
var alert_sound = new Audio('stairs-567.mp3');
var alert_sound_fast = new Audio('stairs-567-fast.mp3');

// summary
let summaryJoint = document.getElementById("summaryJoint");
let summaryFlexion = document.getElementById("summaryFlexion");
let summaryExtension = document.getElementById("summaryExtension");
let summaryRepetition = document.getElementById("summaryRepetition");

myBLE = new p5ble();

new fullpage("#fullpage", {
    sectionsColor: ['#ffffff', '#ffffff', '#ffffff', '#ffffff'],
    sectionSelector: '.vertical-scrolling',
    navigation: true,
    parallax: true,
  });

mailButton.addEventListener('click', function () {
    // input
    let mailSender = document.getElementById("mailInput").value;
    let messageBody =  `<html>
    <h2>Summary of today session</h2>
    <ul>
        <li>Joint: ${jointSelect.value}</li>
        <li>Flexion: ${minReachedValue}</li>
        <li>Extension: ${maxReachedValue}</li>
        <li>Repetitions: 0/${repSelect.value.toString()}</li>
    </ul>
    </html>`;

    // password: 802DD3ADB06ADBE9BCACB005A4BBD1614480
    // security token a83c4b2a-f0fe-4769-b1d2-d486da82b34c
    Email.send({
        SecureToken: "a83c4b2a-f0fe-4769-b1d2-d486da82b34c",
        To: mailSender,
        From: "alert.analyticsperu@gmail.com",
        Subject: "Summary ROM Routine",
        Body: messageBody,
    }).then(
        message => alert("mail has been sent sucessfully")
    );
})

setButton.addEventListener('click', function () {

    // set repetition label
    repLabel.innerHTML = "Repetitions: " + "0/" + repSelect.value.toString();

    // set minimun angle label and variable
    if (breedSelect.value == "labrador") {
        switch (jointSelect.value) {
            case 'carpus':
                minAngleValue = '32';
                maxAngleValue = '196';
                break;
            case 'elbow':
                minAngleValue = '36';
                maxAngleValue = '165';
                break;
            case 'shoulder':
                minAngleValue = '57';
                maxAngleValue = '165';
                break;
            case 'ankle':
                minAngleValue = '39';
                maxAngleValue = '164';
                break;
            case 'stifle':
                minAngleValue = '42';
                maxAngleValue = '162';
                break;
            case 'hip':
                minAngleValue = '50';
                maxAngleValue = '162';
                break;
            default:
                console.log("unknown joint");
        }
    }

    if (breedSelect.value == "german") {
        switch (jointSelect.value) {
            case 'carpus':
                minAngleValue = '34';
                maxAngleValue = '198';
                break;
            case 'elbow':
                minAngleValue = '25';
                maxAngleValue = '155';
                break;
            case 'shoulder':
                minAngleValue = '47';
                maxAngleValue = '159';
                break;
            case 'ankle':
                minAngleValue = '30';
                maxAngleValue = '149';
                break;
            case 'stifle':
                minAngleValue = '33';
                maxAngleValue = '153';
                break;
            case 'hip':
                minAngleValue = '44';
                maxAngleValue = '155';
                break;
            default:
                console.log("unknown joint");
        }
    }

    safeRangeLabel.innerHTML = "Safe range from " + minAngleValue + "º to " + maxAngleValue + "º";
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
    wearableStatus.innerHTML = "Wearable connected";

    // Connect to a device by passing the service UUID
    myBLE.connect(serviceUuid, gotCharacteristics);
}

function disconnectToBle() {
    wearableStatus.innerHTML = "Wearable disconnected";

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
        angleLabel.innerHTML = myValue + "º";

        // change text color
        if ((myValue > 0 && myValue <= parseInt(minAngleValue)) || (myValue >= parseInt(maxAngleValue))) {
            angleLabel.style.color = "red";
            alert_sound_fast.play();

        } else if ((myValue > parseInt(minAngleValue) && myValue <= (parseInt(minAngleValue) + 20)) || (myValue >= (parseInt(maxAngleValue) - 20) && myValue < parseInt(maxAngleValue))) {
            angleLabel.style.color = "orange";
            alert_sound.play();

        } else if (myValue > (parseInt(minAngleValue) + 20) && myValue < (parseInt(maxAngleValue) - 20)) {
            angleLabel.style.color = "green";
        }

        // change mix and max reached value
        if (parseInt(myValue) < parseInt(minReachedValue)) {
            minReachedValue = myValue;
            minReachedLabel.innerHTML = "Flexion: " + minReachedValue + "º";
        }

        if (parseInt(myValue) > parseInt(maxReachedValue)) {
            maxReachedValue = myValue;
            maxReachedLabel.innerHTML = "Extension: " + maxReachedValue + "º";
        }

        // voice feeback
        switch (parseInt(myValue)) {
            case parseInt(minAngleValue):
                min_angle_audio.play();
                break;
            case parseInt(maxAngleValue):
                max_angle_audio.play();
                break;
            default:
                console.log('Not max or min angle.');
        }

        // update summary
        summaryJoint.innerHTML = `Joint: ${jointSelect.value}`;
        summaryFlexion.innerHTML = `Flexion: ${minReachedValue}`;
        summaryExtension.innerHTML = `Extension: ${maxReachedValue}`;
        summaryRepetition.innerHTML = `Repetitions: 0/${repSelect.value}`;
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

playButton.onclick = (event) => {
    chronometerCall = setInterval(chronometer, 1000)
    event.target.setAttribute(`disabled`, ``)
}

pauseButton.onclick = () => {
    clearInterval(chronometerCall)
    playButton.removeAttribute(`disabled`)
}
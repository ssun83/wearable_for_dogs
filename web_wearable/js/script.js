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
let angleSide = false;
let previousAngle = 0;
var contador = 0;

// colors
var normalColor = "#adacb1";
var warningColor = "#f3a712";
var outColor = "#f02bd2";
var blackNormalColor = "#3a3a3a";

// buttons
let connectButton = document.getElementById("connectButton");
let disconnectButton = document.getElementById("disconnectButton");
let startButton = document.getElementById("playButton");
let setButton = document.getElementById("setButton");
let mailButton = document.getElementById("mailButton");
let angleCheckBox = document.getElementById("cbox1");

// selects
let repSelect = document.getElementById("repSelect");
let breedSelect = document.getElementById("breedSelect");
let jointSelect = document.getElementById("jointSelect");
let sideSelect = document.getElementById("sideSelect");
let minCustom = document.getElementById("minCustom");
let maxCustom = document.getElementById("maxCustom");

// labels
let repLabel = document.getElementById("repLabel");
let safeRangeLabel = document.getElementById("safeRangeLabel")
let wearableStatus = document.getElementById("wearableStatus");
let angleLabel = document.getElementById("angleROMLabel");

// spans
var spanText = document.getElementById("spanText");
var spanBarra = document.getElementById("spanBarra");
var toogleText = true;
var toogleBarra = true;

// canvas
var canvas = document.getElementById('canvas');

// audio
var max_angle_audio = new Audio('../assets/full_extension.mp3');
var min_angle_audio = new Audio('../assets/full_flexion.mp3');
var alert_sound = new Audio('../assets/stairs-567.mp3');
var alert_sound_fast = new Audio('../assets/stairs-567-fast.mp3');

// summary
let summaryJoint = document.getElementById("summaryJoint");
let summaryFlexion = document.getElementById("summaryFlexion");
let summaryExtension = document.getElementById("summaryExtension");
let summaryRepetition = document.getElementById("summaryRepetition");
let expectedFlexion = document.getElementById("expectedFlexion");
let expectedExtension = document.getElementById("expectedExtension");

myBLE = new p5ble();

new fullpage("#fullpage", {
    sectionsColor: ['#ffffff', '#ffffff', '#ffffff', '#ffffff'],
    sectionSelector: '.vertical-scrolling',
    navigation: true,
    parallax: true,
});

spanText.addEventListener('click', function () {
    if (toogleText == true) {
        toogleText = false;
        angleLabel.style.display = "none";
        spanText.style.color = "#e1e1e1";
    } else {
        toogleText = true;
        angleLabel.style.display = "block";
        spanText.style.color = "white";
    }
});

spanBarra.addEventListener('click', function () {
    if (toogleBarra == true) {
        toogleBarra = false;
        canvas.style.display = "none";
        spanBarra.style.color = "#e1e1e1";
    } else {
        toogleBarra = true;
        canvas.style.display = "block";
        spanBarra.style.color = "white";
    }
});

mailButton.addEventListener('click', function () {

    var today = new Date();
    var dd = String(today.getDate()).padStart(2, '0');
    var mm = String(today.getMonth() + 1).padStart(2, '0'); //January is 0!
    var yyyy = today.getFullYear();

    today = dd + '/' + mm + '/' + yyyy;

    // input
    let mailSender = 'gabrielalvl31@gmail.com';
    let messageBody = `${jointSelect.value},${minReachedValue},${maxReachedValue},${today}`;

    // password: 802DD3ADB06ADBE9BCACB005A4BBD1614480
    // security token a83c4b2a-f0fe-4769-b1d2-d486da82b34c
    Email.send({
        SecureToken: "a83c4b2a-f0fe-4769-b1d2-d486da82b34c",
        To: mailSender,
        From: "alert.analyticsperu@gmail.com",
        Subject: "romwearable",
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
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'elbow':
                minAngleValue = '36';
                maxAngleValue = '165';
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'shoulder':
                minAngleValue = '57';
                maxAngleValue = '165';
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'ankle':
                minAngleValue = '39';
                maxAngleValue = '164';
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'stifle':
                minAngleValue = '42';
                maxAngleValue = '162';
                if (sideSelect == 'left') {
                    angleSide = true;
                } else {
                    angleSide = false;
                }
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
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'elbow':
                minAngleValue = '25';
                maxAngleValue = '155';
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'shoulder':
                minAngleValue = '47';
                maxAngleValue = '159';
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'ankle':
                minAngleValue = '30';
                maxAngleValue = '149';
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'stifle':
                minAngleValue = '33';
                maxAngleValue = '153';
                if (sideSelect == 'left') {
                    angleSide = true;
                } else {
                    angleSide = false;
                }
                break;
            default:
                console.log("unknown joint");
        }
    }

    if (breedSelect.value == "border-collies") {
        switch (jointSelect.value) {
            case 'carpus':
                minAngleValue = '50';
                maxAngleValue = '205';
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'elbow':
                minAngleValue = '50';
                maxAngleValue = '180';
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'shoulder':
                minAngleValue = '50';
                maxAngleValue = '175';
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'ankle':
                minAngleValue = '50';
                maxAngleValue = '180';
                if (sideSelect == 'left') {
                    angleSide = false;
                } else {
                    angleSide = true;
                }
                break;
            case 'stifle':
                minAngleValue = '50';
                maxAngleValue = '180';
                if (sideSelect == 'left') {
                    angleSide = true;
                } else {
                    angleSide = false;
                }
                break;
            default:
                console.log("unknown joint");
        }
    }

    if (maxCustom.value != 200 && minCustom.value != 10) {
        minAngleValue = minCustom.value.toString();
        maxAngleValue = maxCustom.value.toString();
    }

    safeRangeLabel.innerHTML = "Expected range from " + minAngleValue + "?? to " + maxAngleValue + "??";
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

        // if(Math.abs(parseInt(myValue) - previousAngle) < 30){

        //     previousAngle = parseInt(myValue);
        // }
        // else{
        //     myValue = previousAngle.toString();
        // }

        if (angleSide == true) {
            angleLabel.innerHTML = String(360 - parseInt(myValue)) + "??";
            myValue = String(360 - parseInt(myValue));
        } else {
            angleLabel.innerHTML = myValue + "??";
        }

        // change text color
        if ((myValue > 0 && myValue <= parseInt(minAngleValue)) || (myValue >= parseInt(maxAngleValue))) {
            angleLabel.style.color = outColor;
            alert_sound_fast.play();

        } else if ((myValue > parseInt(minAngleValue) && myValue <= (parseInt(minAngleValue) + 20)) || (myValue >= (parseInt(maxAngleValue) - 20) && myValue < parseInt(maxAngleValue))) {
            angleLabel.style.color = warningColor;
            alert_sound.play();

        } else if (myValue > (parseInt(minAngleValue) + 20) && myValue < (parseInt(maxAngleValue) - 20)) {
            angleLabel.style.color = normalColor;
        }

        if (contador >= 20) {
            // change mix and max reached value
            if (parseInt(myValue) < parseInt(minReachedValue)) {
                minReachedValue = myValue;
            }

            if (parseInt(myValue) > parseInt(maxReachedValue)) {
                maxReachedValue = myValue;
            }
        }
        contador++;

        // voice feeback
        switch (parseInt(myValue)) {
            case parseInt(minAngleValue):
                min_angle_audio.play();
                break;
            case parseInt(maxAngleValue):
                max_angle_audio.play();
                break;
            default:
                break;
        }

        // update summary
        summaryJoint.innerHTML = `Joint: ${jointSelect.value}`;
        summaryFlexion.innerHTML = `${minReachedValue}`;
        summaryExtension.innerHTML = `${maxReachedValue}`;
        summaryRepetition.innerHTML = `Repetitions: ${repSelect.value}`;
        expectedFlexion.innerHTML = `${minAngleValue}`;
        expectedExtension.innerHTML = `${maxAngleValue}`;


        // change draw
        var stops = [
            [parseInt(minAngleValue), outColor], //rojo

            [parseInt(minAngleValue), warningColor], // naranja
            [parseInt(minAngleValue) + 20, warningColor], // naranja

            [parseInt(minAngleValue) + 20, normalColor], // verde
            [parseInt(maxAngleValue) - 20, normalColor], // verde

            [parseInt(maxAngleValue) - 20, warningColor], // naranja
            [parseInt(maxAngleValue), warningColor], // naranja

            [parseInt(maxAngleValue), outColor] // rojo
        ]

        new myGuage.LinearGauge(canvas, 0, 220)
            .draw(stops)
            .drawPointer(parseInt(myValue), '#fff', 20, minAngleValue, maxAngleValue);

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

(function (myGuage) {

    'use strict';

    myGuage.LinearGauge = myGuage.LinearGauge || {};

    // constructor 
    myGuage.LinearGauge = function (canvas, inputLow, inputHigh) {

        this.canvas = canvas;
        this.inputLow = inputLow;
        this.inputHigh = inputHigh;
        this.canvasWidth = Number(canvas.getAttribute("Width"));
        this.canvasHeight = Number(canvas.getAttribute("height"));
    }

    myGuage.LinearGauge.prototype = {

        constructor: myGuage.LinearGauge,

        translateRange: function (
            Input, inputHigh, inputLow, outputHigh, OutputLow
        ) {

            inputHigh = inputHigh ? inputHigh : this.inputHigh;
            inputLow = inputLow ? inputLow : this.inputLow;

            outputHigh = outputHigh ? outputHigh : 1;
            OutputLow = OutputLow ? OutputLow : 0;

            return ((Input - inputLow) / (inputHigh - inputLow)) *
                (outputHigh - OutputLow) + OutputLow;
        },

        draw: function (stops) {

            // setup drawing context
            var ctx = this.canvas.getContext("2d");

            // define the gradient
            var gradient = ctx.createLinearGradient(
                0, 0, this.canvasWidth, 0
            );

            // draw stops from an array
            // where every item is an array contains
            // the position and the color of the gradient
            for (var i = 0; i < stops.length; i++) {
                gradient.addColorStop(
                    this.translateRange(stops[i][0]),
                    stops[i][1]
                );
            }

            // defines the fill style on canvas 
            ctx.fillStyle = gradient;



            // draw the a rect filled with created gradient
            ctx.fillRect(0, 0, this.canvasWidth, this.canvasHeight);


            return this;
        },

        drawPointer: function (value, color, height, item1, item2) {


            // setup drawing context
            var ctx = canvas.getContext("2d");

            height = height ? height : 10;
            ctx.strokeStyle = color ? color : '#000';
            ctx.lineWidth = 4;
            ctx.shadowOffsetX = 2;
            ctx.shadowOffsetY = 2;
            ctx.shadowBlur = 7;
            ctx.shadowColor = "gray";

            // draw line indicate a value 
            ctx.beginPath();
            ctx.moveTo(
                this.translateRange(
                    value,
                    this.inputHigh,
                    this.inputLow,
                    this.canvasWidth,
                    0
                ),
                0
            );
            ctx.lineTo(
                this.translateRange(
                    value,
                    this.inputHigh,
                    0,
                    this.canvasWidth,
                    0
                ),
                height
            );

            ctx.font = '14px Verdana';
            ctx.fillStyle = '#fff';
            // ctx.fillText("15", ((this.canvasWidth / 200) * 10), (this.canvasHeight / 2.3));
            // ctx.fillText("45", ((this.canvasWidth / 200) * 45), (this.canvasHeight / 2.3));
            // ctx.fillText("90", ((this.canvasWidth / 200) * 90), (this.canvasHeight / 2.3));
            // ctx.fillText("135", ((this.canvasWidth / 200) * 135), (this.canvasHeight / 2.3));
            // ctx.fillText("180", ((this.canvasWidth / 200) * 180), (this.canvasHeight / 2.3));

            ctx.fillText(item1, ((this.canvasWidth / 220) * parseInt(item1)), (this.canvasHeight / 2.3));
            ctx.fillText(item2, ((this.canvasWidth / 220) * parseInt(item2)), (this.canvasHeight / 2.3));

            ctx.stroke();

            return this;
        }
    }
}(window.myGuage = window.myGuage || {}));
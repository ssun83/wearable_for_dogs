var textoActivado = document.getElementById("textFeedback");
var toogleVar = true;
var myText = document.getElementById("texto");
var myText2 = document.getElementById("barra");

myText.addEventListener("click", function(){
    if(toogleVar == true){
        toogleVar = false;
        myText2.style.display = "none";
        myText.style.color = "gray";
        console.log("Texto desactivado");
    }
    else{
        toogleVar = true;
        myText2.style.display = "block";
        myText.style.color = "black";
        console.log("texto activado");
    }
});
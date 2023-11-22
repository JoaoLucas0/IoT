
const firebaseConfig = {
	apiKey: "AIzaSyAZpkHJWzImibYA5CoYVgZkFzzOgCIJvJQ",
	authDomain: "trabalhodeiot.firebaseapp.com",
	databaseURL: "https://trabalhodeiot-default-rtdb.firebaseio.com",
	projectId: "trabalhodeiot",
	storageBucket: "trabalhodeiot.appspot.com",
	messagingSenderId: "147003522334",
	appId: "1:147003522334:web:9140ec7938bafc584add0f",
    measurementId: "G-3ZGGYPV4LW"
};


firebase.initializeApp(firebaseConfig)
 

var database = firebase.database()

function save(){
var input = document.getElementById("input").value

database.ref('input/').set({
        input : input

})
alert('salvo')
}

function get(){
    var input = document.getElementById('input').value

var input_ref = database.ref("input/")
input_ref.on("value", function(snapshot){
            var data = snapshot.val()

        })
}

function update(){
        var input = document.getElementById('input').value
        
        var updates = {
            input : 'input'
        } 
    
    database.ref('input/').update(updates)
    alert("atualizado")
    }    



    


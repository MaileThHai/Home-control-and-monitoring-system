
  const firebaseConfig = {
    apiKey: "AIzaSyAZx6zK3WP8eXC6kAEJIonxL_Mtmxxi750",
    authDomain: "kltn-bd6e3.firebaseapp.com",
    databaseURL: "https://kltn-bd6e3-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "kltn-bd6e3",
    storageBucket: "kltn-bd6e3.appspot.com",
    messagingSenderId: "710366745217",
    appId: "1:710366745217:web:55d6e68d1aa5f83d123380",
  };

  // Initialize Firebase
  firebase.initializeApp(firebaseConfig);
// ==============================================DATA NODE 1==============================================
  
firebase.database().ref("/KLTN/node1/sensor/Temp").on("value", function(snapshot) {
    var nd = snapshot.val();  
    document.getElementById("temp_node1").innerHTML = nd;
    console.log(nd);
});

firebase.database().ref("/KLTN/node1/sensor/Humidity").on("value", function(snapshot) {
    var nd = snapshot.val();  
    document.getElementById("hum_node1").innerHTML = nd;
    console.log(nd);
});
firebase.database().ref("/KLTN/node1/sensor/CO").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("co2_node1").innerHTML = nd;
  console.log(nd);
});
// ==============================================DATA NODE 2==============================================
firebase.database().ref("/KLTN/node2/sensor/Temp").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("temp_node2").innerHTML = nd;
  console.log(nd);
});

firebase.database().ref("/KLTN/node2/sensor/Humidity").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("hum_node2").innerHTML = nd;
  console.log(nd);
});

firebase.database().ref("/KLTN/node2/sensor/CO").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("co2_node2").innerHTML = nd;
  console.log(nd);
});
// ==============================================DATA NODE 3==============================================
firebase.database().ref("/KLTN/node3/sensor/Temp").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("temp_node3").innerHTML = nd;
  console.log(nd);
});

firebase.database().ref("/KLTN/node3/sensor/Humidity").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("hum_node3").innerHTML = nd;
  console.log(nd);
});

firebase.database().ref("/KLTN/node3/sensor/Rain").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("rain_node3").innerHTML = nd;
  console.log(nd);
});

firebase.database().ref("/KLTN/node3/sensor/Hum_soil").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("humsoil_node3").innerHTML = nd;
  console.log(nd);
});


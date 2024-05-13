
  // Initialize Firebase
firebase.initializeApp(firebaseConfig);

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
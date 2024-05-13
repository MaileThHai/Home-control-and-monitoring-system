

  // Initialize Firebase
  firebase.initializeApp(firebaseConfig);
  // Data node1
  // Auto load Temperature-------------------------
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
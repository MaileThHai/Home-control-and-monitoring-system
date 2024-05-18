

  // Initialize Firebase
  firebase.initializeApp(firebaseConfig);
  // Auto load Temperature-------------------------
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
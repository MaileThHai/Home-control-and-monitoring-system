
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
// ==============================================DATA SENSOR NODE 1==============================================
  
firebase.database().ref("/KLTN/node1/sensor/Temp").on("value", function(snapshot) {
    var nd = snapshot.val();  
    document.getElementById("temp_node1").innerHTML = nd;
    console.log(nd);
    // effect when data change
    var icon = document.getElementById("temp1");
    var data = document.getElementById("temp_node1");
    icon.classList.add("vibrate");
    data.classList.add("blink","color_change");
    setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
    }, 500);
});

firebase.database().ref("/KLTN/node1/sensor/Humidity").on("value", function(snapshot) {
    var nd = snapshot.val();  
    document.getElementById("hum_node1").innerHTML = nd;
    console.log(nd);
    var icon = document.getElementById("hum1");
    var data = document.getElementById("hum_node1","color_change");
    icon.classList.add("vibrate");
    data.classList.add("blink","color_change");
    setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
    }, 500);
});
firebase.database().ref("/KLTN/node1/sensor/CO").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("co2_node1").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("co1");
  var data = document.getElementById("co2_node1");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
    icon.classList.remove("vibrate");
    data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node1/sensor/TVCO").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("tvco_node1").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("tvco1");
  var data = document.getElementById("tvco_node1");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
    }, 500);
});

firebase.database().ref("/KLTN/node1/sensor/Fire").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("fire_node1").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("fire1");
  var data = document.getElementById("fire_node1");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node1/sensor/Smoke").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("smoke_node1").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("smoke1");
  var data = document.getElementById("smoke_node1");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});
// ============================================== RELAY NODE 1==============================================
firebase.database().ref("/KLTN/node1/Relay1").on("value", function(snapshot) {
  var relay1_status = snapshot.val();
  console.log(relay1_status);
  //Relay 01
  document.getElementById("relay1_node1").innerHTML = relay1_status
  console.log("updated relay1_node1");
  var icon = document.getElementById("img_relay1_node1");
  var data = document.getElementById("relay1_node1");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});


firebase.database().ref("/KLTN/node1/Relay2").on("value", function(snapshot) {
var relay2_status = snapshot.val();
console.log(relay2_status);
  //Relay 02
  document.getElementById("relay2_node1").innerHTML = relay2_status
  console.log("updated relay2_node1");
  var icon = document.getElementById("img_relay2_node1");
  var data = document.getElementById("relay2_node1");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});
firebase.database().ref("/KLTN/node1/Relay3").on("value", function(snapshot) {
  var relay3_status = snapshot.val();
  console.log(relay3_status);
    //Relay 02
    document.getElementById("relay3_node1").innerHTML = relay3_status
    console.log("updated relay3_node1");
    var icon = document.getElementById("img_relay3_node1");
    var data = document.getElementById("relay3_node1");
    icon.classList.add("vibrate");
    data.classList.add("blink","color_change");
    setTimeout(function() {
        icon.classList.remove("vibrate");
        data.classList.remove("blink","color_change");
    }, 500);
  });
firebase.database().ref("/KLTN/node1/Relay4").on("value", function(snapshot) {
  var relay4_status = snapshot.val();
  console.log(relay4_status);
    //Relay 02
    document.getElementById("relay4_node1").innerHTML = relay4_status
    console.log("updated relay4_node1");
    var icon = document.getElementById("img_relay4_node1");
    var data = document.getElementById("relay4_node1");
    icon.classList.add("vibrate");
    data.classList.add("blink","color_change");
    setTimeout(function() {
        icon.classList.remove("vibrate");
        data.classList.remove("blink","color_change");
    }, 500);
  });
firebase.database().ref("/KLTN/node1/Loa").on("value", function(snapshot) {
  var speaker_status = snapshot.val();
  console.log(speaker_status);
  //Relay 01
  document.getElementById("speaker_node1").innerHTML = speaker_status
  console.log("updated speaker");
  var icon = document.getElementById("speaker1");
  var data = document.getElementById("speaker_node1");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});
// ==============================================DATA NODE 2==============================================
firebase.database().ref("/KLTN/node2/sensor/Temp").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("temp_node2").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("temp2");
  var data = document.getElementById("temp_node2");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node2/sensor/Humidity").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("hum_node2").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("hum2");
  var data = document.getElementById("hum_node2");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node2/sensor/CO").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("co2_node2").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("co2");
  var data = document.getElementById("co2_node2");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node2/sensor/TVCO").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("tvco_node2").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("tvco2");
  var data = document.getElementById("tvco_node2");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node1/sensor/Fire").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("fire_node2").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("fire2");
  var data = document.getElementById("fire_node2");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node1/sensor/Smoke").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("smoke_node2").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("smoke2");
  var data = document.getElementById("smoke_node2");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});
// ============================================== RELAY NODE 2==============================================
firebase.database().ref("/KLTN/node2/Relay1").on("value", function(snapshot) {
  var relay1_node2_status = snapshot.val();
  console.log(relay1_node2_status);
  //Relay 01
  document.getElementById("relay1_node2").innerHTML = relay1_node2_status
  console.log("updated relay1_node2");
  var icon = document.getElementById("img_relay1_node2");
  var data = document.getElementById("relay1_node2");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node2/Relay2").on("value", function(snapshot) {
var relay2_node2_status = snapshot.val();
console.log(relay2_node2_status);
  //Relay 02
  document.getElementById("relay2_node2").innerHTML = relay2_node2_status
  console.log("updated relay2_node2");
  var icon = document.getElementById("img_relay2_node2");
  var data = document.getElementById("relay2_node2");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});
firebase.database().ref("/KLTN/node2/Relay3").on("value", function(snapshot) {
var relay3_node2_status = snapshot.val();
console.log(relay3_node2_status);
  //Relay 03
  document.getElementById("relay3_node2").innerHTML = relay3_node2_status
  console.log("updated relay3_node2");
  var icon = document.getElementById("img_relay3_node2");
  var data = document.getElementById("relay3_node2");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node2/Relay4").on("value", function(snapshot) {
var relay4_node2_status = snapshot.val();
console.log(relay4_node2_status);
  //Relay 04
  document.getElementById("relay4_node2").innerHTML = relay4_node2_status
  console.log("updated relay4_node2");
  var icon = document.getElementById("img_relay4_node2");
  var data = document.getElementById("relay4_node2");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

// ==============================================DATA NODE 3==============================================
firebase.database().ref("/KLTN/node3/sensor/Temp").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("temp_node3").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("temp3");
  var data = document.getElementById("temp_node3");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node3/sensor/Humidity").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("hum_node3").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("hum3");
  var data = document.getElementById("hum_node3");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node3/sensor/Rain").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("rain_node3").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("rain3");
  var data = document.getElementById("rain_node3");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node3/sensor/Hum_soil").on("value", function(snapshot) {
  var nd = snapshot.val();  
  document.getElementById("humsoil_node3").innerHTML = nd;
  console.log(nd);
  var icon = document.getElementById("humsoil3");
  var data = document.getElementById("humsoil_node3");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});
// ============================================== RELAY NODE 3==============================================
firebase.database().ref("/KLTN/node3/Relay1").on("value", function(snapshot) {
  var relay1_node3_status = snapshot.val();
  console.log(relay1_node3_status);
  //Relay 01
  document.getElementById("relay1_node3").innerHTML = relay1_node3_status
  console.log("updated relay1_node3");
  var icon = document.getElementById("img_relay1_node3");
  var data = document.getElementById("relay1_node3");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

firebase.database().ref("/KLTN/node3/Relay2").on("value", function(snapshot) {
var relay2_node3_status = snapshot.val();
console.log(relay2_node3_status);
  //Relay 02
  document.getElementById("relay2_node3").innerHTML = relay2_node3_status
  console.log("updated relay2_node3");
  var icon = document.getElementById("img_relay2_node3");
  var data = document.getElementById("relay2_node3");
  icon.classList.add("vibrate");
  data.classList.add("blink","color_change");
  setTimeout(function() {
      icon.classList.remove("vibrate");
      data.classList.remove("blink","color_change");
  }, 500);
});

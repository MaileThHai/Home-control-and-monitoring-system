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
  // Data node1
  // Auto load Temperature-------------------------
var temp_node1 = [];
var hum_node1 = [];
var co2_node1 = [];
var tvco_node1 = [];


firebase.database().ref("/KLTN/node1/sensor/Temp").on("value", function(snapshot) {
    var temp = snapshot.val();
    temp_node1.push(temp);  
    document.getElementById("temp_node1").innerHTML = temp;
    console.log("temp_node1: "+temp);
});

firebase.database().ref("/KLTN/node1/sensor/Humidity").on("value", function(snapshot) {
    var hum = snapshot.val(); 
    hum_node1.push(hum); 
    document.getElementById("hum_node1").innerHTML = hum;
    console.log("hum_node1: "+hum);
});

firebase.database().ref("/KLTN/node1/sensor/CO").on("value", function(snapshot) {
    var co = snapshot.val();  
    co2_node1.push(co);
    document.getElementById("co2_node1").innerHTML = co;
    console.log("co2_node1: "+co);
});

firebase.database().ref("/KLTN/node1/sensor/TVCO").on("value", function(snapshot) {
    var tvco = snapshot.val(); 
    tvco_node1.push(tvco); 
    document.getElementById("tvco_node1").innerHTML = tvco;
    console.log("tvco_node1: "+tvco);
});

firebase.database().ref("/KLTN/node1/sensor/Fire").on("value", function(snapshot) {
    var nd = snapshot.val();  
    document.getElementById("fire_node1").innerHTML = nd;
    console.log("fire_node1: " + nd);
});

firebase.database().ref("/KLTN/node1/sensor/Smoke").on("value", function(snapshot) {
    var nd = snapshot.val();  
    document.getElementById("smoke_node1").innerHTML = nd;
    console.log("smoke_node1: " + nd);
  });

firebase.database().ref("/KLTN/node1/Relay1").on("value", function(snapshot) {
      var relay1_status = snapshot.val();
      console.log("firebase relay1 status: " + relay1_status);
      //Relay 01
      if (relay1_status == "ON"){
        document.getElementById("relay1_node1_id").src = "./image/relayon.png"
        document.getElementById("relay1_node1").innerHTML = "ON"
        relay1_on.classList.add("click_on");
        relay1_off.classList.remove("click_off");
      }else if (relay1_status == "OFF"){
        document.getElementById("relay1_node1_id").src = "./image/relayoff.png"
        document.getElementById("relay1_node1").innerHTML = "OFF"
        relay1_off.classList.add("click_off");
        relay1_on.classList.remove("click_on");
      }
      console.log("updated relay1");
});
firebase.database().ref("/KLTN/node1/Relay2").on("value", function(snapshot) {
    var relay2_status = snapshot.val();
    console.log("firebase relay2 status: " + relay2_status);
      //Relay 02
      if (relay2_status == "ON"){
        document.getElementById("relay2_node1_id").src = "./image/relayon.png"
        document.getElementById("relay2_node1").innerHTML = "ON"
        relay2_on.classList.add("click_on");
        relay2_off.classList.remove("click_off");
      }else if (relay2_status == "OFF"){
        document.getElementById("relay2_node1_id").src = "./image/relayoff.png"
        document.getElementById("relay2_node1").innerHTML = "OFF"
        relay2_off.classList.add("click_off");
        relay2_on.classList.remove("click_on");
      }
      console.log("updated relay2");
});
firebase.database().ref("/KLTN/node1/Relay3").on("value", function(snapshot) {
    var relay3_status = snapshot.val();
    console.log("firebase relay3 status: " + relay3_status);
      //Relay 04
      if (relay3_status == "ON"){
        document.getElementById("relay3_node1_id").src = "./image/relayon.png"
        document.getElementById("relay3_node1").innerHTML = "ON"
        relay3_on.classList.add("click_on");
        relay3_off.classList.remove("click_off");
      }else if (relay3_status == "OFF"){
        document.getElementById("relay3_node1_id").src = "./image/relayoff.png"
        document.getElementById("relay3_node1").innerHTML = "OFF"
        relay3_off.classList.add("click_off");
        relay3_on.classList.remove("click_on");
      }
      console.log("updated relay3");
});
firebase.database().ref("/KLTN/node1/Relay4").on("value", function(snapshot) {
    var relay4_status = snapshot.val();
    console.log("firebase relay4 status: " + relay4_status);
      //Relay 04
      if (relay4_status == "ON"){
        document.getElementById("relay4_node1_id").src = "./image/relayon.png"
        document.getElementById("relay4_node1").innerHTML = "ON"
        relay4_on.classList.add("click_on");
        relay4_off.classList.remove("click_off");
      }else if (relay4_status == "OFF"){
        document.getElementById("relay4_node1_id").src = "./image/relayoff.png"
        document.getElementById("relay4_node1").innerHTML = "OFF"
        relay4_off.classList.add("click_off");
        relay4_on.classList.remove("click_on");
      }
      console.log("updated relay4");
});
firebase.database().ref("/KLTN/node1/Loa").on("value", function(snapshot) {
    var speaker_status = snapshot.val();
    console.log("firebase speaker status: " + speaker_status);
    //Loa
    if (speaker_status == "ON"){
      document.getElementById("speaker_node1_id").src = "./image/speaker_on.png"
      document.getElementById("speaker_node1").innerHTML = "ON"
    }else if (speaker_status == "OFF"){
      document.getElementById("speaker_node1_id").src = "./image/speaker_off.png"
      document.getElementById("speaker_node1").innerHTML = "OFF"
    }
    console.log("updated speaker");
});
var relay1_on = document.getElementById("relay1_node1_on");
var relay1_off = document.getElementById("relay1_node1_off");

relay1_on.onclick = function(){
    console.log("Relay1 On");
    document.getElementById("relay1_node1_id").src = "./image/relayon.png";
    document.getElementById("relay1_node1").innerHTML = "ON";
    firebase.database().ref("/KLTN/node1").update({
    "Relay1": "ON"
  });
  relay1_on.classList.add("click_on");
  relay1_off.classList.remove("click_off");
};

relay1_off.onclick = function(){
    console.log("Relay1 Off");
	document.getElementById("relay1_node1_id").src = "./image/relayoff.png"
	document.getElementById("relay1_node1").innerHTML = "OFF"
    firebase.database().ref("/KLTN/node1").update({
	"Relay1": "OFF"
	});
    relay1_off.classList.add("click_off");
    relay1_on.classList.remove("click_on");
};

var relay2_on = document.getElementById("relay2_node1_on");
var relay2_off = document.getElementById("relay2_node1_off");

relay2_on.onclick = function(){
    console.log("Relay2 On");
    document.getElementById("relay2_node1_id").src = "./image/relayon.png";
    document.getElementById("relay2_node1").innerHTML = "ON";
    firebase.database().ref("/KLTN/node1").update({
    "Relay2": "ON"
  });
  relay2_on.classList.add("click_on");
  relay2_off.classList.remove("click_off");
};

relay2_off.onclick = function(){
    console.log("Relay2 Off");
	document.getElementById("relay2_node1_id").src = "./image/relayoff.png"
	document.getElementById("relay2_node1").innerHTML = "OFF"
    firebase.database().ref("/KLTN/node1").update({
	"Relay2": "OFF"
	});
    relay2_off.classList.add("click_off");
    relay2_on.classList.remove("click_on");
};
var relay3_on = document.getElementById("relay3_node1_on");
var relay3_off = document.getElementById("relay3_node1_off");

relay3_on.onclick = function(){
    console.log("Relay3 On");
    document.getElementById("relay3_node1_id").src = "./image/relayon.png";
    document.getElementById("relay3_node1").innerHTML = "ON";
    firebase.database().ref("/KLTN/node1").update({
    "Relay3": "ON"
  });
  relay3_on.classList.add("click_on");
  relay3_off.classList.remove("click_off");
};

relay3_off.onclick = function(){
    console.log("Relay3 Off");
	document.getElementById("relay3_node1_id").src = "./image/relayoff.png"
	document.getElementById("relay3_node1").innerHTML = "OFF"
    firebase.database().ref("/KLTN/node1").update({
	"Relay3": "OFF"
	});
    relay3_off.classList.add("click_off");
    relay3_on.classList.remove("click_on");
};

var relay4_on = document.getElementById("relay4_node1_on");
var relay4_off = document.getElementById("relay4_node1_off");

relay4_on.onclick = function(){
    console.log("Relay4 On");
    document.getElementById("relay4_node1_id").src = "./image/relayon.png";
    document.getElementById("relay4_node1").innerHTML = "ON";
    firebase.database().ref("/KLTN/node1").update({
    "Relay4": "ON"
  });
  relay4_on.classList.add("click_on");
  relay4_off.classList.remove("click_off");
};

relay4_off.onclick = function(){
    console.log("Relay4 Off");
    document.getElementById("relay4_node1_id").src = "./image/relayoff.png";
    document.getElementById("relay4_node1").innerHTML = "OFF";
    firebase.database().ref("/KLTN/node1").update({
    "Relay4": "OFF"
  });
    relay4_off.classList.add("click_off");
    relay4_on.classList.remove("click_on");
};
var speaker_on = document.getElementById("speaker_node1_on");
var speaker_off = document.getElementById("speaker_node1_off");

speaker_on.onclick = function(){
    console.log("Speaker On");
    document.getElementById("speaker_node1_id").src = "./image/speaker_on.png";
    document.getElementById("speaker_node1").innerHTML = "ON";
    firebase.database().ref("/KLTN/node1").update({
    "Loa": "ON"
  });
  speaker_on.classList.add("click_on");
  speaker_off.classList.remove("click_off");
};

speaker_off.onclick = function(){
    console.log("Speaker Off");
	document.getElementById("speaker_node1_id").src = "./image/speaker_off.png"
	document.getElementById("speaker_node1").innerHTML = "OFF"
    firebase.database().ref("/KLTN/node1").update({
	"Loa": "OFF"
	});
    speaker_off.classList.add("click_off");
    speaker_on.classList.remove("click_on");
};
let tempchart = new Chart(temperatureChartCanvas, {
  type: 'line',
  data: {
      labels: timeLabels,
      datasets: [{
          label: 'Temperature',
          data: temp_node1,
          borderColor: 'rgba(255, 99, 132, 1)',
          backgroundColor: 'rgba(255, 99, 132, 0.2)',
          fill: true
      }]
  },
  options: {
      responsive: true,
      scales: {
          x: {
              ticks: {
                  font: {
                      size: 10 // adjust this value to change the font size
                  }
              }
          },
          y: {
              ticks: {
                  font: {
                      size: 10 // adjust this value to change the font size
                  }
              }
          }
      }
  }
});
let humichart = new Chart(humidityChartCanvas, {
  type: 'line',
  data: {
      labels: timeLabels,
      datasets: [{
          label: 'Humidity',
          data: hum_node1,
          borderColor: 'blue',
          backgroundColor: 'rgba(185, 249, 244, 0.8)',
          fill: true
      }]
  },
  options: {
      responsive: true,
      scales: {
          x: {
              ticks: {
                  font: {
                      size: 10 // adjust this value to change the font size
                  }
              }
          },
          y: {
              ticks: {
                  font: {
                      size: 10 // adjust this value to change the font size
                  }
              }
          }
      }
  }
});
let cochart =new Chart(co2ChartCanvas, {
  type: 'line',
  data: {
      labels: timeLabels,
      datasets: [{
          label: 'CO2',
          data: co2_node1,
          borderColor: 'orange',
          backgroundColor: 'rgba(250, 232, 198, 0.8)',
          fill: true
      }]
  },
  options: {
    responsive: true,
      scales: {
          x: {
              ticks: {
                  font: {
                      size: 10 // adjust this value to change the font size
                  }
              }
          },
          y: {
              ticks: {
                  font: {
                      size: 10 // adjust this value to change the font size
                  }
              }
          }
      }
  }
});
let tvcochart = new Chart(TVCOChartCanvas, {
  type: 'line',
  data: {
      labels: timeLabels,
      datasets: [{
          label: 'CO2',
          data: tvco_node1,
          borderColor: 'green',
          backgroundColor: 'rgba(205, 250, 198, 0.8)',
          fill: true
      }]
  },
  options: {
    responsive: true,
      scales: {
          x: {
              ticks: {
                  font: {
                      size: 10 // adjust this value to change the font size
                  }
              }
          },
          y: {
              ticks: {
                  font: {
                      size: 10 // adjust this value to change the font size
                  }
              }
          }
      }
  }
}); 

// update data chart
function updateData() {
  
  // Add new time label
  let now = new Date();
  let time = `${now.getHours()}:${now.getMinutes()}:${now.getSeconds()}`;
  //console.log('updateData called, current time:', time);
  timeLabels.push(time);
  temp_node1.push(temp_node1[temp_node1.length - 1]);
  hum_node1.push(hum_node1[hum_node1.length - 1]);
  co2_node1.push(co2_node1[co2_node1.length - 1]);
  tvco_node1.push(tvco_node1[tvco_node1.length - 1]);
  // Remove the first item in the array if it has more than 10 items
  if (temp_node1.length > 8) {
    timeLabels.shift();
    temp_node1.shift();
    hum_node1.shift();
    co2_node1.shift();
    tvco_node1.shift();
  }

  // Update the chart
  tempchart.update();
  humichart.update();
  cochart.update();
  tvcochart.update();
}

// Call updateData every second
setInterval(updateData, 1000);
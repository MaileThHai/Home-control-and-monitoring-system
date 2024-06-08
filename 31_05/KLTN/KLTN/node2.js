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
firebase.auth().onAuthStateChanged((user) => {
  if (user) {
    console.log('User is logged in');
    console.log(user.uid);
    const logout = document.querySelector('#sidebar-logout');
    logout.addEventListener('click', function(event) {
      event.preventDefault();
      firebase.auth().signOut().then(() => {
        // get current date and time
        const now = new Date();
        const logoutTime = now.toString();
        console.log('Logout time: ' + logoutTime);
  
        console.log('About to add logout time to Realtime Database');
        // save logout time to Realtime Database
        firebase.database().ref("/KLTN/logins/user").push({
          logoutTime: logoutTime
        });
        window.location.href = 'login.html';
      }).catch((error) => {
        console.error(error);
      });
    });
    const sidebarLogout = document.querySelector('#sidebar-logout'); // replace '#sidebar-logout' with the actual id of your sidebar logout button
    sidebarLogout.addEventListener('click', function(event) {
    event.preventDefault();
    firebase.auth().signOut().then(() => {
      // get current date and time
      const now = new Date();
      const logoutTime = now.toString();
      console.log('Logout time: ' + logoutTime);
      const user = firebase.auth().currentUser;
      if(user) {
        const uid = user.uid;
        console.log('About to add logout time to Realtime Database');
        // save logout time to Realtime Database
        firebase.database().ref("/KLTN/logins/user" + ": " + uid).push({
          logoutTime: logoutTime
        });
      }
      window.location.href = 'login.html';
    }).catch((error) => {
      console.error(error);
    });
  });
    var temp_node2 = [];
    var hum_node2 = [];
    var co2_node2 = [];
    var tvoc_node2 = [];
    // -------------------------------Auto load Data -------------------------
    firebase.database().ref("/KLTN/node2/sensor/Temp").on("value", function (snapshot) {
      var temp = snapshot.val();
      temp_node2.push(temp);
      document.getElementById("temp_node2").innerHTML = temp;
      console.log(temp);
    });

    firebase.database().ref("/KLTN/node2/sensor/Humidity").on("value", function (snapshot) {
      var nd = snapshot.val();
      hum_node2.push(nd);
      document.getElementById("hum_node2").innerHTML = nd;
      console.log(nd);
    });

    firebase.database().ref("/KLTN/node2/sensor/CO2").on("value", function (snapshot) {
      var nd = snapshot.val();
      co2_node2.push(nd);
      document.getElementById("co2_node2").innerHTML = nd;
      console.log(nd);
    });

    firebase.database().ref("/KLTN/node2/sensor/TVOC").on("value", function (snapshot) {
      var nd = snapshot.val();
      tvoc_node2.push(nd);
      document.getElementById("tvoc_node2").innerHTML = nd;
      console.log(nd);
    });

    firebase.database().ref("/KLTN/node2/sensor/Fire").on("value", function (snapshot) {
      var nd = snapshot.val();
      document.getElementById("fire_node2").innerHTML = nd;
      console.log(nd);
    });

    firebase.database().ref("/KLTN/node2/Relay2_1").on("value", function (snapshot) {
      var relay1_status = snapshot.val();
      console.log("firebase relay1 status: " + relay1_status);
      //Relay 01
      if (relay1_status == "ON") {
        document.getElementById("relay1_node2_id").src = "./image/relayon.png"
        document.getElementById("relay1_node2").innerHTML = "ON"
        relay1_on.classList.add("click_on");
        relay1_off.classList.remove("click_off");
      } else if (relay1_status == "OFF") {
        document.getElementById("relay1_node2_id").src = "./image/relayoff.png"
        document.getElementById("relay1_node2").innerHTML = "OFF"
        relay1_off.classList.add("click_off");
        relay1_on.classList.remove("click_on");
      }
      console.log("updated relay1");
    });
    firebase.database().ref("/KLTN/node2/Relay2_2").on("value", function (snapshot) {
      var relay2_status = snapshot.val();
      console.log("firebase relay2 status: " + relay2_status);
      //Relay 02
      if (relay2_status == "ON") {
        document.getElementById("relay2_node2_id").src = "./image/relayon.png"
        document.getElementById("relay2_node2").innerHTML = "ON"
        relay2_on.classList.add("click_on");
        relay2_off.classList.remove("click_off");
      } else if (relay2_status == "OFF") {
        document.getElementById("relay2_node2_id").src = "./image/relayoff.png"
        document.getElementById("relay2_node2").innerHTML = "OFF"
        relay2_off.classList.add("click_off");
        relay2_on.classList.remove("click_on");
      }
      console.log("updated relay2");
    });
    firebase.database().ref("/KLTN/node2/Relay2_3").on("value", function (snapshot) {
      var relay3_status = snapshot.val();
      console.log("firebase relay3 status: " + relay3_status);
      //Relay 03
      if (relay3_status == "ON") {
        document.getElementById("relay3_node2_id").src = "./image/relayon.png"
        document.getElementById("relay3_node2").innerHTML = "ON"
        relay3_on.classList.add("click_on");
        relay3_off.classList.remove("click_off");
      } else if (relay3_status == "OFF") {
        document.getElementById("relay3_node2_id").src = "./image/relayoff.png"
        document.getElementById("relay3_node2").innerHTML = "OFF"
        relay3_off.classList.add("click_off");
        relay3_on.classList.remove("click_on");
      }
      console.log("updated relay3");
    });
    firebase.database().ref("/KLTN/node2/Relay2_4").on("value", function (snapshot) {
      var relay4_status = snapshot.val();
      console.log("firebase relay4 status: " + relay4_status);
      //Relay 04
      if (relay4_status == "ON") {
        document.getElementById("relay4_node2_id").src = "./image/relayon.png"
        document.getElementById("relay4_node2").innerHTML = "ON"
        relay4_on.classList.add("click_on");
        relay4_off.classList.remove("click_off");
      } else if (relay4_status == "OFF") {
        document.getElementById("relay4_node2_id").src = "./image/relayoff.png"
        document.getElementById("relay4_node2").innerHTML = "OFF"
        relay4_off.classList.add("click_off");
        relay4_on.classList.remove("click_on");
      }
      console.log("updated relay4");
    });
    var relay1_on = document.getElementById("relay1_node2_on");
    var relay1_off = document.getElementById("relay1_node2_off");

    relay1_on.onclick = function () {
      console.log("Relay1 On");
      document.getElementById("relay1_node2_id").src = "./image/relayon.png";
      document.getElementById("relay1_node2").innerHTML = "ON";
      firebase.database().ref("/KLTN/node2").update({
        "Relay2_1": "ON"
      });
      relay1_on.classList.add("click_on");
      relay1_off.classList.remove("click_off");
    };

    relay1_off.onclick = function () {
      console.log("Relay1 Off");
      document.getElementById("relay1_node2_id").src = "./image/relayoff.png"
      document.getElementById("relay1_node2").innerHTML = "OFF"
      firebase.database().ref("/KLTN/node2").update({
        "Relay2_1": "OFF"
      });
      relay1_off.classList.add("click_off");
      relay1_on.classList.remove("click_on");
    };

    var relay2_on = document.getElementById("relay2_node2_on");
    var relay2_off = document.getElementById("relay2_node2_off");

    relay2_on.onclick = function () {
      console.log("Relay2 On");
      document.getElementById("relay2_node2_id").src = "./image/relayon.png";
      document.getElementById("relay2_node2").innerHTML = "ON";
      firebase.database().ref("/KLTN/node2").update({
        "Relay2_2": "ON"
      });
      relay2_on.classList.add("click_on");
      relay2_off.classList.remove("click_off");
    };

    relay2_off.onclick = function () {
      console.log("Relay2 Off");
      document.getElementById("relay2_node2_id").src = "./image/relayoff.png"
      document.getElementById("relay2_node2").innerHTML = "OFF"
      firebase.database().ref("/KLTN/node2").update({
        "Relay2_2": "OFF"
      });
      relay2_off.classList.add("click_off");
      relay2_on.classList.remove("click_on");
    };
    var relay3_on = document.getElementById("relay3_node2_on");
    var relay3_off = document.getElementById("relay3_node2_off");

    relay3_on.onclick = function () {
      console.log("Relay3 On");
      document.getElementById("relay3_node2_id").src = "./image/relayon.png";
      document.getElementById("relay3_node2").innerHTML = "ON";
      firebase.database().ref("/KLTN/node2").update({
        "Relay2_3": "ON"
      });
      relay3_on.classList.add("click_on");
      relay3_off.classList.remove("click_off");
    };

    relay3_off.onclick = function () {
      console.log("Relay3 Off");
      document.getElementById("relay3_node2_id").src = "./image/relayoff.png"
      document.getElementById("relay3_node2").innerHTML = "OFF"
      firebase.database().ref("/KLTN/node2").update({
        "Relay2_3": "OFF"
      });
      relay3_off.classList.add("click_off");
      relay3_on.classList.remove("click_on");
    };
    var relay4_on = document.getElementById("relay4_node2_on");
    var relay4_off = document.getElementById("relay4_node2_off");

    relay4_on.onclick = function () {
      console.log("Relay4 On");
      document.getElementById("relay4_node2_id").src = "./image/relayon.png";
      document.getElementById("relay4_node2").innerHTML = "ON";
      firebase.database().ref("/KLTN/node2").update({
        "Relay2_4": "ON"
      });
      relay4_on.classList.add("click_on");
      relay4_off.classList.remove("click_off");
    };
    relay4_off.onclick = function () {
      console.log("Relay4 Off");
      document.getElementById("relay4_node2_id").src = "./image/relayoff.png"
      document.getElementById("relay4_node2").innerHTML = "OFF"
      firebase.database().ref("/KLTN/node2").update({
        "Relay2_4": "OFF"
      });
      relay4_off.classList.add("click_off");
      relay4_on.classList.remove("click_on");
    };
    let tempchart = new Chart(temperatureChartCanvas, {
      type: 'line',
      data: {
        labels: timeLabels,
        datasets: [{
          label: 'Temperature',
          data: temp_node2,
          borderColor: 'rgba(255, 99, 132, 1)',
          backgroundColor: 'rgba(255, 99, 132, 0.2)',
          fill: true
        }]
      },
      options: {
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
          data: hum_node2,
          borderColor: 'blue',
          backgroundColor: 'rgba(185, 249, 244, 0.8)',
          fill: true
        }]
      },
      options: {
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
    let cochart = new Chart(co2ChartCanvas, {
      type: 'line',
      data: {
        labels: timeLabels,
        datasets: [{
          label: 'CO2',
          data: co2_node2,
          borderColor: 'orange',
          backgroundColor: 'rgba(250, 232, 198, 0.8)',
          fill: true
        }]
      },
      options: {
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
    let tvocchart = new Chart(TVOCChartCanvas, {
      type: 'line',
      data: {
        labels: timeLabels,
        datasets: [{
          label: 'CO2',
          data: tvoc_node2,
          borderColor: 'green',
          backgroundColor: 'rgba(205, 250, 198, 0.8)',
          fill: true
        }]
      },
      options: {
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

      timeLabels.push(time);
      temp_node2.push(temp_node2[temp_node2.length - 1]);
      hum_node2.push(hum_node2[hum_node2.length - 1]);
      co2_node2.push(co2_node2[co2_node2.length - 1]);
      tvoc_node2.push(tvoc_node2[tvoc_node2.length - 1]);
      // Remove the first item in the array if it has more than 10 items
      if (temp_node2.length > 8) {

        timeLabels.shift();
        temp_node2.shift();
        hum_node2.shift();
        co2_node2.shift();
        tvoc_node2.shift();
      }

      // Update the chart
      tempchart.update();
      humichart.update();
      cochart.update();
      tvocchart.update();
    }

    // Call updateData every second
    setInterval(updateData, 1000);
  } else {
    console.log('User is not logged in');
    window.location.href = 'login.html';
  }
});

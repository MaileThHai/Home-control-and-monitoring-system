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
    logout.addEventListener('click', function (event) {
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
    sidebarLogout.addEventListener('click', function (event) {
      event.preventDefault();
      firebase.auth().signOut().then(() => {
        // get current date and time
        const now = new Date();
        const logoutTime = now.toString();
        console.log('Logout time: ' + logoutTime);
        const user = firebase.auth().currentUser;
        if (user) {
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
    var temp_node3 = [];
    var hum_node3 = [];
    var humsoil_node3 = [];
    firebase.database().ref("/KLTN/node3/sensor/Temp").on("value", function (snapshot) {
      var nd = snapshot.val();
      temp_node3.push(nd);
      document.getElementById("temp_node3").innerHTML = nd;
      console.log("temp_node3: " + nd);
    });

    firebase.database().ref("/KLTN/node3/sensor/Humidity").on("value", function (snapshot) {
      var nd = snapshot.val();
      hum_node3.push(nd);
      document.getElementById("hum_node3").innerHTML = nd;
      console.log("hum_node3: " + nd);
    });

    firebase.database().ref("/KLTN/node3/sensor/Rain").on("value", function (snapshot) {
      var nd = snapshot.val();
      document.getElementById("rain_node3").innerHTML = nd;
      console.log("rain_node3: " + nd);
    });

    firebase.database().ref("/KLTN/node3/sensor/Hum_soil").on("value", function (snapshot) {
      var nd = snapshot.val();
      humsoil_node3.push(nd);
      document.getElementById("humsoil_node3").innerHTML = nd;
      console.log("humsoil_node3: " + nd);
    });

    firebase.database().ref("/KLTN/node3/Relay3_1").on("value", function (snapshot) {
      var relay1_status = snapshot.val();
      console.log("firebase relay1_node3 status: " + relay1_status);
      //Relay 01
      if (relay1_status == "ON") {
        document.getElementById("relay1_node3_id").src = "./image/relayon.png"
        document.getElementById("relay1_node3").innerHTML = "ON"
        relay1_on.classList.add("click_on");
        relay1_off.classList.remove("click_off");
      } else if (relay1_status == "OFF") {
        document.getElementById("relay1_node3_id").src = "./image/relayoff.png"
        document.getElementById("relay1_node3").innerHTML = "OFF"
        relay1_off.classList.add("click_off");
        relay1_on.classList.remove("click_on");
      }
      console.log("updated relay1_node3");
    });
    firebase.database().ref("/KLTN/node3/Relay3_2").on("value", function (snapshot) {
      var relay2_status = snapshot.val();
      console.log("firebase relay2_node3 status: " + relay2_status);
      //Relay 02
      if (relay2_status == "ON") {
        document.getElementById("relay2_node3_id").src = "./image/relayon.png"
        document.getElementById("relay2_node3").innerHTML = "ON"
        relay2_on.classList.add("click_on");
        relay2_off.classList.remove("click_off");
      } else if (relay2_status == "OFF") {
        document.getElementById("relay2_node3_id").src = "./image/relayoff.png"
        document.getElementById("relay2_node3").innerHTML = "OFF"
        relay2_off.classList.add("click_off");
        relay2_on.classList.remove("click_on");
      }
      console.log("updated relay2_node3");
    });

    var relay1_on = document.getElementById("relay1_node3_on");
    var relay1_off = document.getElementById("relay1_node3_off");

    relay1_on.onclick = function () {
      console.log("Relay1_node3 On");
      document.getElementById("relay1_node3_id").src = "./image/relayon.png";
      document.getElementById("relay1_node3").innerHTML = "ON";
      firebase.database().ref("/KLTN/node3").update({
        "Relay3_1": "ON"
      });
      relay1_on.classList.add("click_on");
      relay1_off.classList.remove("click_off");
    };

    relay1_off.onclick = function () {
      console.log("Relay1_node3 Off");
      document.getElementById("relay1_node3_id").src = "./image/relayoff.png"
      document.getElementById("relay1_node3").innerHTML = "OFF"
      firebase.database().ref("/KLTN/node3").update({
        "Relay3_1": "OFF"
      });
      relay1_off.classList.add("click_off");
      relay1_on.classList.remove("click_on");
    };

    var relay2_on = document.getElementById("relay2_node3_on");
    var relay2_off = document.getElementById("relay2_node3_off");

    relay2_on.onclick = function () {
      console.log("Relay2_node3 On");
      document.getElementById("relay2_node3_id").src = "./image/relayon.png";
      document.getElementById("relay2_node3").innerHTML = "ON";
      firebase.database().ref("/KLTN/node3").update({
        "Relay3_2": "ON"
      });
      relay2_on.classList.add("click_on");
      relay2_off.classList.remove("click_off");
    };

    relay2_off.onclick = function () {
      console.log("Relay2_node3 Off");
      document.getElementById("relay2_node3_id").src = "./image/relayoff.png"
      document.getElementById("relay2_node3").innerHTML = "OFF"
      firebase.database().ref("/KLTN/node3").update({
        "Relay3_2": "OFF"
      });
      relay2_off.classList.add("click_off");
      relay2_on.classList.remove("click_on");
    };

    let tempchart = new Chart(temperatureChartCanvas, {
      type: 'line',
      data: {
        labels: timeLabels,
        datasets: [{
          label: 'Temperature',
          data: temp_node3,
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
          data: hum_node3,
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

    let humsoilchart = new Chart(co2ChartCanvas, {
      type: 'line',
      data: {
        labels: timeLabels,
        datasets: [{
          label: 'Độ ẩm đất',
          data: humsoil_node3,
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
                size: 5 // adjust this value to change the font size
              }
            }
          },
          y: {
            ticks: {
              font: {
                size: 5 // adjust this value to change the font size
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
      temp_node3.push(temp_node3[temp_node3.length - 1]);
      hum_node3.push(hum_node3[hum_node3.length - 1]);
      humsoil_node3.push(humsoil_node3[humsoil_node3.length - 1]);
      // Remove the first item in the array if it has more than 10 items
      if (temp_node3.length > 10) {

        timeLabels.shift();
        temp_node3.shift();
        hum_node3.shift();
        humsoil_node3.shift();
      }

      // Update the chart
      tempchart.update();
      humichart.update();
      humsoilchart.update();
    }
    setInterval(updateData, 1000);
  } else {
    console.log('User is not logged in');
    window.location.href = 'login.html';
  }
});

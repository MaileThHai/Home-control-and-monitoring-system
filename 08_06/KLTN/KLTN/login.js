const loginForm = document.getElementById('login-form');
const errorMessage = document.getElementById('error-message');
const eyeIcon = document.getElementById('eye-icon');
// export logintime so that it can be used in other files
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


loginForm.addEventListener('submit', function(event) {
  event.preventDefault();

  const email = document.getElementById('email').value;
  const password = document.getElementById('password').value;
// sign up
  firebase.auth().createUserWithEmailAndPassword(email, password)
    .then((userCredential) => {
      // Signed in
      var user = userCredential.user;
      // Redirect to 'Layout.html' after the login time is successfully added
      window.location.href = 'index.html';
      // ...
    })
    .catch((error) => {
      var errorCode = error.code;
      var errorMessage = error.message;
    });  
// sign in
  firebase.auth().signInWithEmailAndPassword(email, password)
    .then((userCredential) => {
     // Get current date and time
    const now = new Date();
    const loginTime = now.toString();
    const uid = userCredential.user.uid;
    
    console.log('About to add login time to Realtime Database');

    // Save login time to Realtime Database
    firebase.database().ref("/KLTN/logins/user").set({
      loginTime: loginTime
    }).then(() => {
      console.log("Login time successfully added!")
      window.location.href = 'index.html';}
    );      
    })
    .catch((error) => {
      var errorCode = error.code;
      var errorMessage = error.message;
    });
});

// Hiển thị mật khẩu khi nhấn vào biểu tượng con mắt
eyeIcon.addEventListener('click', function() {
  if (passwordInput.type === 'password') {
    passwordInput.type = 'text';
    eyeIcon.classList.remove('fa-eye');
    eyeIcon.classList.add('fa-eye-slash');
  } else {
    passwordInput.type = 'password';
    eyeIcon.classList.remove('fa-eye-slash');
    eyeIcon.classList.add('fa-eye');
  }
});

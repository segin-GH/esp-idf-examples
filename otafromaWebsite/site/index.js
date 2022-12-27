const form = document.forms["otaForm"];
const fileInput = document.getElementById("fileInput");
let file;
let checkedValue;

const radioButtons = document.getElementsByName("updateType");
for (let i = 0; i < radioButtons.length; i++) {
  radioButtons[i].addEventListener("change", () => {
    checkedValue = radioButtons[i].value;
  });
}

fileInput.addEventListener("change", () => {
  file = fileInput.files[0];
});

const uploadFile = (file) => {
  // Create an XMLHttpRequest object
  const xhr = new XMLHttpRequest();

  // Declare variables to store the form data and the endpoint
  let formData;
  let endpoint;
  let contentType;

  // Check the value of checkedValue and set the form data, endpoint, and content type accordingly
  if (checkedValue === "binary") {
    formData = file;
    endpoint = "/ota";
    contentType = "application/octet-stream";
  } else {
    // Create a FormData object
    formData = new FormData();
    // Append the file to the FormData object
    formData.append("textFile", file);
    endpoint = "/spiffs";
    contentType = "multipart/form-data";
  }

  xhr.open("POST", endpoint);
  xhr.setRequestHeader("Content-Type", contentType);

  // Add an event listener for the "progress" event
  xhr.upload.addEventListener("progress", (event) => {
    const progress = Math.floor((event.loaded / event.total) * 100);
    document.getElementById("progress-percentage").innerHTML = `${progress}%`;
  });

  xhr.onload = () => {
    // Check the status of the response
    if (xhr.status === 200) {
      console.log(xhr.responseText);
      window.alert(xhr.responseText);
    } else {
      console.error("Error uploading file!");
      window.alert(xhr.responseText);
    }
  };

  // Send the request
  xhr.send(formData);
  console.log(xhr.responseText);
};

form.addEventListener("submit", (e) => {
  e.preventDefault();
  uploadFile(file);
});

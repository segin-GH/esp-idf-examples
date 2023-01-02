const form = document.forms["otaForm"];
const fileInput = document.getElementById("fileInput");
let file;
let checkedValue="binary";

const radioButtons = document.getElementsByName("updateType");
for (let i = 0; i < radioButtons.length; i++) {
  radioButtons[i].addEventListener("change", () => {
    checkedValue = radioButtons[i].value;
    document.getElementById("progress-percentage").innerHTML = `0%`;
  });
}

fileInput.addEventListener("change", () => {
  file = fileInput.files[0];
});

const uploadFile = (file) => {

  const xhr = new XMLHttpRequest();
if(checkedValue==="binary"){
  xhr.open("POST", "/ota");
  xhr.setRequestHeader("Content-Type", "application/octet-stream");
  xhr.onload = () => {
    // Check the status of the response
    if (xhr.status === 200) {
      console.log(xhr.responseText);
      window.alert(xhr.responseText);
    } else {
      console.error("Error uploading file!");
      window.alert(xhr.responseText);
    } };
    xhr.upload.addEventListener("progress", (event) => {
      const progress = Math.floor((event.loaded / event.total) * 100);
      document.getElementById("progress-percentage").innerHTML = `${progress}%`;
    }); 
  xhr.send(file);
  console.log(xhr.responseText);
  
}else if( checkedValue==="fs"){
  xhr.open("POST", "/spiffs");
  formData = new FormData();
 formData.append("textFile", file);
  xhr.setRequestHeader("Content-Type", "multipart/form-data");
  xhr.onload = () => {
    // Check the status of the response
    if (xhr.status === 200) {
      console.log(xhr.responseText);
      window.alert(xhr.responseText);
    } else {
      console.error("Error uploading file!");
      window.alert(xhr.responseText);
    } };
    xhr.upload.addEventListener("progress", (event) => {
      const progress = Math.floor((event.loaded / event.total) * 100);
      document.getElementById("progress-percentage").innerHTML = `${progress}%`;
    }); 
  xhr.send(formData);
  console.log(xhr.responseText);
}
};

form.addEventListener("submit", (e) => {
  e.preventDefault();
  uploadFile(file);
});
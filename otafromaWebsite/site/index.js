const form = document.forms["otaForm"];
const fileInput = document.getElementById("fileInput");
const URL = "/ota";
let file;

fileInput.addEventListener("change", () => {
  file = fileInput.files[0];
});

const uploadFile = (file) => {
  const xhr = new XMLHttpRequest();
  xhr.open("POST", URL);
  xhr.setRequestHeader("Content-Type", "application/octet-stream");
  xhr.upload.addEventListener("progress", (event) => {
    const progress = Math.floor((event.loaded / event.total) * 100);
    const progressElement = document.getElementById("progress-percentage");
    progressElement.innerHTML = `${progress}%`;
  });
  xhr.onload = () => {
    if (xhr.status === 200) {
      console.log(xhr.responseText);
      window.alert(xhr.responseText);
    } else {
      console.error("Error uploading file!");
      window.alert(xhr.responseText);
    }
  };
  xhr.send(file);

console.log(xhr.responseText);
};


form.addEventListener("submit", (e) => {
  e.preventDefault();
  uploadFile(file);
});

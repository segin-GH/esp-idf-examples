const form = document.forms["otaForm"];
const fileInput = document.getElementById("fileInput");
const URL = "http://esp-server.local/update";

let file;

console.log("my dear monu")
fileInput.addEventListener("change", () => {
  file = fileInput.files[0];
});

const uploadFile = (file) => {
  const fd = new FormData();
  fd.append("otaFile", file);
  console.log(fd);
  const xhr = new XMLHttpRequest();
  xhr.open("POST", "URL");
  xhr.upload.addEventListener("progress", ({ loaded, total }) => {
    const fileLoaded = Math.floor((loaded / total) * 100);
    const progress = document.getElementById("progress-percentage");
    progress.innerHTML = `${fileLoaded}%`;
  });
  xhr.send(fd);
};

form.addEventListener("submit", (e) => {
  e.preventDefault();
  uploadFile(file);
});

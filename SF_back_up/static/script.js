Webcam.attach('#camera');
  var image_taken;
  var statusCode = 0;

  function take_photo() {
	  Webcam.snap( function(data_uri) {
		  document.getElementById("camera").innerHTML = '<img src="'+data_uri+'"/>';
      image_taken = data_uri;
      document.getElementById("capture-btn").style.display = "none";
      document.getElementById("ac").style.display = "inline";
	  } );
  }

  function s(){
    if(image_taken != null){
      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/image", true);
      xhr.send(image_taken);
      loading();
    }
    else{alert("Take a photo first");}
  }


function loading() {
  document.getElementById("box").style.display = "none";
  document.getElementById("loader_container").style.display = "block";
  var intervalId = setInterval(function() {
    fetch('/is_done')
      .then(function(response) {
        statusCode = response.status;

        if (statusCode === 200) {
          clearInterval(intervalId);
          window.location.replace('/attended');          
        }
      })
      .catch(function() {
        clearInterval(intervalId);
      });
  }, 750);
}

let currentPage = 1;
const itemsPerPage = 10;

function filterStudents() {
    let input = document.getElementById('search').value.toLowerCase();
    let listItems = document.querySelectorAll('.student-list li');
    listItems.forEach(item => {
        let name = item.querySelector('span').innerText.toLowerCase();
        item.style.display = name.includes(input) ? '' : 'none';
    });
}

function showPage(page) {
    let listItems = document.querySelectorAll('.student-list li');
    let totalPages = Math.ceil(listItems.length / itemsPerPage);
    listItems.forEach((item, index) => {
        item.style.display = index >= (page - 1) * itemsPerPage && index < page * itemsPerPage ? '' : 'none';
    });
    document.getElementById('page-number').innerText = `Page ${page}`;
    currentPage = page;
}

function prevPage() {
    if (currentPage > 1) showPage(currentPage - 1);
}

function nextPage() {
    let totalPages = Math.ceil(document.querySelectorAll('.student-list li').length / itemsPerPage);
    if (currentPage < totalPages) showPage(currentPage + 1);
}

showPage(currentPage);
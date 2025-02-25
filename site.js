var i = 0;
document.getElementById("btn").addEventListener("click", () => {
    document.getElementById("headr").innerHTML = `click time ${i}`;
    fetch("/randomnumber").then(res => res.json()).then(data=>document.getElementById("headr").innerHTML = `${data}!`);   
    i++;
})


document.getElementById("postBtn").addEventListener("click",()=>{
    fetch("/postnumber",{
        method:"POST",
        body:document.getElementById("postText").value.toString(),
    }).then(res=> res.text()).then(data=>document.getElementById("headr").innerHTML = `${data}!`);
})
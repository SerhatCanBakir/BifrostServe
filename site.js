var i = 0;
document.getElementById("btn").addEventListener("click", () => {
    document.getElementById("headr").innerHTML = `click time ${i}`;
    fetch("/randomnumber").then(res => res.text()).then(data=>document.getElementById("headr").innerHTML = `click time ${data.toString()}`);   
    i++;
})
const sign_up = document.getElementById("signUp");
const send_otp = document.getElementById("otp");

function generateOTP() {
    return Math.floor(100000 + Math.random() * 900000); // Mã OTP 6 chữ số
}
// The OTP code will be used for verification.
let otp_code = 0;
// Attemps to enter OTP Code
let attempts = 0; 

send_otp.addEventListener('click', function(e)
{   e.preventDefault();
    const random_number = generateOTP();
    const to_Email = 'dangkhoa123.2004@gmail.com';
    emailjs.send("service_r39cr7m","template_p8ez63k",{
        message: random_number.toString(),
        to_Email: to_Email,
    }).then(response => {
        alert("The OTP will be sent to your email.");
        otp_code = random_number;   
    });
});

sign_up.addEventListener('submit', function (e) 
{
    e.preventDefault();
    // Get Name of User
    const firstName = document.getElementById("FirstName").value;
    const lastName = document.getElementById("LastName").value;
    // File Image Handling
    const image = document.getElementById("imageInput");
    const file = image.files[0];
    const fileType = file.type.split('/')[1];
    // OTP Code
    const code = document.getElementById("CodeOTP").value;
    // Bucket to storge Image and API of Bucket
    const bucket_name = 'registrate-storage'
    const api_url = `https://xl0lcefokd.execute-api.ap-southeast-2.amazonaws.com/my_stage/${bucket_name}/${firstName}_${lastName}.${fileType}`;
    if (otp_code == 0)
    {
        alert("Kindly ensure that you retrieve the OTP before proceeding with the submission.")
    }
    else if (code == otp_code)
    {
        fetch(api_url, {
            method: 'PUT',
            body: file,
        }).then(response => {
            alert("File uploaded successfully!");
        }).catch(error => {
            alert("Error uploading file:", error);
        });
        attempts = 0;
        location.reload();
    }
    else
    {
        attempts++;
        if (attempts >= 5) {
            emailjs.send("service_r39cr7m","template_4oh0buj",{
                to_email: "dangkhoa123.2004@gmail.com",
            }).then(response => {
                alert("Please ensure that you have the host's permission before registering.");
                location.reload(); 
            });
        }
        else {
            alert("Incorrect OTP. You have " + (5 - attempts) + " attempts remaining.");
        }
    }
});
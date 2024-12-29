import boto3
import json
import base64
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email import encoders

# AWS clients
s3 = boto3.client('s3')
rekognition = boto3.client('rekognition', region_name='ap-southeast-2')
dynamodb = boto3.resource('dynamodb', region_name='ap-southeast-2')
registrationTable = dynamodb.Table('people-registration')
iot_client = boto3.client('iot-data', region_name='ap-southeast-2')

# Email credentials
smtp_port = 587                 # SMTP port
smtp_server = "smtp.gmail.com"   # Google SMTP Server
from_email = "dangkhoa123.2004@gmail.com"  # Sender email
password = "axmrssbexfulngrj"   # App-specific password
to_email = "ledangkhoagg@gmail.com"  # Recipient email

# IoT topic
iot_topic = 'esp32/sub'

def lambda_handler(event, context):
    print(event)
    objectKey = event['Records'][0]['s3']['object']['key']
    bucketName = event['Records'][0]['s3']['bucket']['name']
    
    # Retrieve image bytes from S3
    image_bytes = s3.get_object(Bucket=bucketName, Key=objectKey)['Body'].read()

    # Perform face recognition
    response = rekognition.search_faces_by_image(
        CollectionId='registor',
        Image={'Bytes': image_bytes}
    )

    for match in response.get('FaceMatches', []):
        print(match['Face']['FaceId'], match['Face']['Confidence'])
        face = registrationTable.get_item(
            Key={
                'rekognitionId': match['Face']['FaceId'],
            }
        )

        if 'Item' in face:
            print('Person Found: ', face['Item'])
            sendSignalToIoT("open_door")
            buildAndSendEmail(200, {
                'Message': 'Success',
                'firstName': face['Item']['firstName'],
                'lastName': face['Item']['lastName']
            }, image_bytes)
            return

    print('Person could not recognize')
    sendSignalToIoT("close_door")
    buildAndSendEmail(403, {'Message': 'Person not found'}, image_bytes)

def sendSignalToIoT(signal):
    try:
        payload = json.dumps({'action': signal})
        iot_client.publish(
            topic=iot_topic,
            qos=0,
            payload=payload
        )
        print(f"Signal '{signal}' sent to IoT device.")
    except Exception as e:
        print(f"Error sending signal to IoT device: {e}")

def buildAndSendEmail(statusCode, body, imageBytes):
    # Determine the file name for the image
    if statusCode == 200 and 'firstName' in body and 'lastName' in body:
        file_name = f"{body['firstName']}_{body['lastName']}.jpg"
    else:
        file_name = "unknown_person.jpg"

    # Email content
    subject = "Face Recognition Result"
    body_message = f"Status Code: {statusCode}\n\n{json.dumps(body, indent=4)}"

    # Create email
    msg = MIMEMultipart()
    msg['From'] = from_email
    msg['To'] = to_email
    msg['Subject'] = subject
    msg.attach(MIMEText(body_message, 'plain'))

    # Attach image (mandatory)
    try:
        attachment = MIMEBase('application', 'octet-stream')
        attachment.set_payload(imageBytes)
        encoders.encode_base64(attachment)
        attachment.add_header('Content-Disposition', f'attachment; filename={file_name}')
        msg.attach(attachment)
    except Exception as e:
        print(f"Error attaching image: {e}")

    # Send email
    try:
        server = smtplib.SMTP(smtp_server, smtp_port)
        server.starttls()
        server.login(from_email, password)
        server.sendmail(from_email, to_email, msg.as_string())
        print("Email sent successfully!")
    except Exception as e:
        print(f"Error sending email: {e}")
    finally:
        server.quit()
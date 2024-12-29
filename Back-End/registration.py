import boto3

s3 = boto3.client('s3')
rekognition = boto3.client('rekognition', region_name='ap-southeast-2')
dynamodbTableName = 'people-registration'
dynamodb = boto3.resource('dynamodb', region_name='ap-southeast-2')
registrationTable = dynamodb.Table(dynamodbTableName)

def lambda_handler(event, text):
    print(event)
    bucket = event['Records'][0]['s3']['bucket']['name']
    key = event['Records'][0]['s3']['object']['key']

    try:
        respone = index_registration_image(bucket, key)
        print(respone)
        if respone['ResponseMetadata']['HTTPStatusCode'] == 200:
            faceId = respone['FaceRecords'][0]['Face']['FaceId']
            name  = key.split('.')[0].split('_')
            firstName = name[0]
            lastName = name[1]
            register(faceId, firstName, lastName)
        return respone
    except Exception as e:
        print(e)
        print('Error processing registor image {} from bucket{}.' .format(key, bucket))
        raise e


def index_registration_image(bucket, key):
    respone = rekognition.index_faces(
        Image={
            'S3Object':{
                'Bucket' : bucket,
                'Name' : key
            }
        },
        CollectionId = "registor" # Do later
    )

    return respone

def register(faceId, firstName, lastName):
    registrationTable.put_item(
        Item={
            'rekognitionId' : faceId,
            'firstName' : firstName,
            'lastName' : lastName
        }
    )

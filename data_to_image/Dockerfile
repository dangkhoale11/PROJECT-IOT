FROM public.ecr.aws/lambda/python:3.12

COPY requirements.txt ${LAMBDA_TASK_ROOT}

RUN pip install --upgrade pip

RUN pip install -r requirements.txt

COPY handler.py ${LAMBDA_TASK_ROOT}


CMD ["handler.image"]
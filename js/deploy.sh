#!/bin/bash

AWS_UID=$(aws sts get-caller-identity --query Account --output text)

IMG_NAME="phonomask-build"
REPO_NAME="phonomask-repo"

# docker build -t phonomask-build .

docker logout $AWS_UID.dkr.ecr.us-east-2.amazonaws.com

# Deploy to ECR (example)
aws ecr get-login-password\
	--region us-east-2\
|\
docker login\
	--username AWS \
	--password-stdin $AWS_UID.dkr.ecr.us-east-2.amazonaws.com 


# aws ecr create-repository --repository-name $REPO_NAME --region us-east-2


# docker tag $IMG_NAME:latest $AWS_UID.dkr.ecr.us-east-2.amazonaws.com/$REPO_NAME:latest
docker tag $IMG_NAME:latest\
	$AWS_UID.dkr.ecr.us-east-2.amazonaws.com/$REPO_NAME:latest

docker push $AWS_UID.dkr.ecr.us-east-2.amazonaws.com/$REPO_NAME:latest



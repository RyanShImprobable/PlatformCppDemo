// PlatformDemo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "improbable/spatialos/deployment/v1alpha1/deployment.grpc.pb.h"
#include "improbable/spatialos/deployment/v1alpha1/deployment.pb.h"
#include <grpcpp/grpcpp.h>
#include <fstream>
#include <string>

using grpc::Channel;
using grpc::ChannelArguments;
using grpc::ClientContext;
using grpc::Status;
using improbable::spatialos::deployment::v1alpha1::DeploymentService;
using improbable::spatialos::deployment::v1alpha1::CreateDeploymentRequest;
using improbable::spatialos::deployment::v1alpha1::Deployment;
using improbable::spatialos::deployment::v1alpha1::LaunchConfig;

std::string readFileToString(const std::string filePath)
{
	std::ifstream ifs(filePath);
	std::string content((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	return content;
}


int main()
{
	//read launchConfig
	std::string content = readFileToString("C:\\UE\\UnrealEngineWithGdk\\Samples\\UnrealGDKExampleProject\\spatial\\default_launch.json");
	std::cout << content;
	
	ChannelArguments args;
	auto* dlp = new Deployment();
	auto* launch_config = new LaunchConfig();
	
	launch_config->set_config_json(content);
	dlp->set_project_name("your_project_name_here");
	dlp->set_name("test_local_deployment");
	dlp->set_allocated_launch_config(launch_config);

	
	auto cd_req = CreateDeploymentRequest();
	cd_req.set_allocated_deployment(dlp);
	
	DeploymentService::Stub d_stub = DeploymentService::Stub(grpc::CreateCustomChannel(
		"localhost:9876", grpc::InsecureChannelCredentials(), args));
	
	ClientContext context;
    google::longrunning::Operation response;
	
	d_stub.CreateDeployment(&context, cd_req, &response);
}




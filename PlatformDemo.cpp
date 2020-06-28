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
using improbable::spatialos::deployment::v1alpha1::ListDeploymentsRequest;
using improbable::spatialos::deployment::v1alpha1::Deployment;
using improbable::spatialos::deployment::v1alpha1::LaunchConfig;
using improbable::spatialos::deployment::v1alpha1::ListDeploymentsResponse;

std::string readFileToString(const std::string filePath)
{
	std::ifstream ifs(filePath);
	std::string content((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	return content;
}

void checkAndDelete(const std::string project_name, DeploymentService::Stub d_stub)
{
	ClientContext context;
	google::longrunning::Operation response;

	auto ld_req = ListDeploymentsRequest();
	ld_req.set_project_name(project_name);
	ld_req.set_deployment_stopped_status_filter(ListDeploymentsRequest::NOT_STOPPED_DEPLOYMENTS);

	auto ld_res = ListDeploymentsResponse();
	d_stub.ListDeployments(&context, ld_req, &ld_res);

	google::protobuf::RepeatedPtrField<Deployment> deployment_list = ld_res.deployments();
	for (auto it = deployment_list.begin(); it != deployment_list.end(); it++)
	{
		auto dp = *it;
		ClientContext inner_context;

		if (project_name == dp.project_name())
		{
			std::cout << "Project found!";
			auto delet_req = improbable::spatialos::deployment::v1alpha1::DeleteDeploymentRequest();
			delet_req.set_id(dp.id());
			d_stub.DeleteDeployment(&inner_context, delet_req, &response);
		}
	}
}

void creatDeployment(const std::string project_name, std::string launch_config_path, DeploymentService::Stub d_stub)
{
	ClientContext context;
	google::longrunning::Operation response;
	std::string content = readFileToString("C:\\UE\\UnrealEngineWithGdk\\Samples\\UnrealGDKExampleProject\\spatial\\default_launch.json");
	auto* dlp = new Deployment();
	auto* launch_config = new LaunchConfig();

	launch_config->set_config_json(content);
	dlp->set_project_name(project_name);
	dlp->set_name("test_local_deployment");
	dlp->set_allocated_launch_config(launch_config);

	auto cd_req = CreateDeploymentRequest();
	cd_req.set_allocated_deployment(dlp);
	d_stub.CreateDeployment(&context, cd_req, &response);
	//response.error().message()
}

int main()
{
	const std::string project_name = "your_project_name_here";
	ChannelArguments args;
	DeploymentService::Stub d_stub = DeploymentService::Stub(grpc::CreateCustomChannel(
		"localhost:9876", grpc::InsecureChannelCredentials(), args));
	
	checkAndDelete(project_name, d_stub);

	std::string launch_path = "C:\\UE\\UnrealEngineWithGdk\\Samples\\UnrealGDKExampleProject\\spatial\\default_launch.json";
	creatDeployment(project_name, launch_path, d_stub);
}






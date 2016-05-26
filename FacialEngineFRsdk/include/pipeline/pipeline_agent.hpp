#ifndef PipelineAgent_INCLUDED
#define PipelineAgent_INCLUDED

#include <agents.h>
//#include "wrappers/image_info.hpp"
//#include "pipeline_item_holders.hpp"
#include "frsdk_biometric_processor.hpp"
#include "face_info_awaitable.hpp"

namespace Pipeline
{
	class BiometricPipelineAgent : public Concurrency::agent
	{
	private:
		//ISource<bool>& m_cancellationSource;
		//ITarget<ErrorInfo>& m_errorTarget;
		//mutable overwrite_buffer<bool> m_shutdownPending;
		//mutable critical_section m_latestImageLock;

	public:
		explicit BiometricPipelineAgent(IBiometricProcessorPtr pipeline) : pipeline_(pipeline)
		{
			//send(m_shutdownPending, false);
		}
		//virtual int GetQueueSize(int queue) const = 0;

		bool IsCancellationPending() const { return false /*receive(m_shutdownPending) || receive(m_cancellationSource)*/; }

		FRsdkEntities::ImageInfoPtr load_image(const std::string& filename) const
		{
			FRsdkEntities::ImageInfoPtr pInfo = nullptr;
			try
			{
				if (!IsCancellationPending())
					pInfo = pipeline_->load_image(filename);
			}
			catch (std::exception& e)
			{
				ShutdownOnError(0, filename, e);
			}

			return pInfo;
		}

		void face_find(FRsdkEntities::ImageInfoPtr pInfo) const
		{
			try
			{
				if (!IsCancellationPending() && (nullptr != pInfo))
					pipeline_->face_find(pInfo);

				//std::cout << "face_find" << std::endl;
			}
			catch (std::exception& e)	{
				ShutdownOnError(0, pInfo->filename(), e);
			}
		}

		void iso_compliance_test(FaceInfoAwaitablePtr pInfo) const
		{
			try
			{
				auto face = pInfo->item();
				if (!IsCancellationPending() && (nullptr != face))
				{
					pipeline_->iso_compliance_test(face);
					pInfo->set_done(IsoComplianceTest);
				}

				//std::cout << "iso_compliance_test" << std::endl;
			}
			catch (std::exception& e)	{
				ShutdownOnError(0, "eyes find", e);
			}
		}


		void portrait_analyze(FaceInfoAwaitablePtr pInfo) const
		{
			try
			{
				auto face = pInfo->item();
				if (!IsCancellationPending() && (nullptr != face))
				{
					pipeline_->portrait_analyze(face);
					pInfo->set_done(BiometricTask::PortraitAnalysis);
				}

				//std::cout << "portrait_analyze" << std::endl;
			}
			catch (std::exception& e)	{
				ShutdownOnError(0, "portrait characteristic find", e);
			}
		}

		void extract_facial_image(FaceInfoAwaitablePtr pInfo) const
		{
			try
			{
				auto face = pInfo->item();
				if (!IsCancellationPending() && (nullptr != face))
				{
					pipeline_->extract_facial_image(face);
					pInfo->set_done(FaceImageExtraction);
				}
				//std::cout << "extract_facial_image" << std::endl;
			}
			catch (std::exception& e)	{
				ShutdownOnError(0, "portrait characteristic find", e);
			}
		}

		void enroll(FaceInfoAwaitablePtr pInfo) const
		{
			try
			{
				auto face = pInfo->item();
				if (!IsCancellationPending() && (nullptr != face))
				{
					pipeline_->enroll(face);
					pInfo->set_done(Enrollment);
				}
				//std::cout << "enroll" << std::endl;
			}
			catch (std::exception& e)	{
				ShutdownOnError(0, "portrait characteristic find", e);
			}
		}

		void verify(FRsdkEntities::FaceInfoPtr pInfo) const
		{
			try
			{
				if (!IsCancellationPending() && (nullptr != pInfo))
					pipeline_->verify(pInfo);

				//std::cout << "face_find" << std::endl;
			}
			catch (std::exception& e)	{
				ShutdownOnError(0, "portrait characteristic find", e);
			}
		}

		void finish(FRsdkEntities::FaceInfoPtr pInfo) const
		{
			try
			{
		   
			}
			catch (std::exception& e)	{
				ShutdownOnError(0, "portrait characteristic find", e);
			}
		}


	private:
		void ShutdownOnError(long phase, const FRsdkEntities::ImageInfoPtr pInfo, const std::exception& e) const
		{
			ShutdownOnError(phase, pInfo->filename(), e);
		}

		void ShutdownOnError(long phase, const std::string& filePath, const std::exception& e) const
		{
			std::string message = e.what();
			SendError(phase, filePath, message);
		}

		void SendError(long phase, const std::string& filePath, const std::string message) const
		{
			//send(m_shutdownPending, true);
			//send(m_errorTarget    , ErrorInfo(phase, filePath, message));		
		}

	private:
		IBiometricProcessorPtr pipeline_;
	};
}
#endif
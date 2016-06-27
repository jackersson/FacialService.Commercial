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

		
		void face_find(FRsdkEntities::ImageInfoPtr pInfo) const
		{
			try
			{
				if (!IsCancellationPending() && (nullptr != pInfo))										
					pipeline_->face_find(pInfo);								
			}
			catch (std::exception& e)	{
				ShutdownOnError(0, pInfo->image()->name(), e);
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
					pInfo->set_done(PortraitAnalysis);
				}

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
	
			}
			catch (std::exception& e)	{
				ShutdownOnError(0, "portrait characteristic find", e);
			}
		}

		BioContracts::VerificationResultPtr verify(BioFacialEngine::VerificationPair pInfo) const
		{
			try
			{
				if (!IsCancellationPending())
					return pipeline_->verify(pInfo);
			}
			catch (std::exception& e)	{
				ShutdownOnError(0, "portrait characteristic find", e);
			}

			return nullptr;
		}

		BioContracts::IdentificationResultPtr identify(BioFacialEngine::IdentificationPair pInfo) const
		{
			try
			{
				if (!IsCancellationPending())
					return pipeline_->identify(pInfo);
			}
			catch (std::exception& e)	{
				ShutdownOnError(0, "portrait characteristic find", e);
			}

			return nullptr;
		}

		
		long create_identification_population(std::list<FRsdkEntities::ImageInfoPtr> images) const
		{				
			return pipeline_->create_identification_population(images);
		}
		


	private:
		void ShutdownOnError(long phase, const FRsdkEntities::ImageInfoPtr pInfo, const std::exception& e) const
		{
			ShutdownOnError(phase, pInfo->image()->name(), e);
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
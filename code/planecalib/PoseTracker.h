
#ifndef POSETRACKER_H_
#define POSETRACKER_H_

#include <memory>
#include <vector>
#include <list>
#include <unordered_map>

#include "Map.h"
#include "FeatureMatcher.h"
#include "PoseEstimationCommon.h"


namespace planecalib {

///////////////////////////////////
// Classes
class Keyframe;

class PoseTracker
{
public:
	void init(const Eigen::Vector2i &imageSize, int octaveCount);
	void resetTracking(Map *map, const Eigen::Matrix3fr &initialPose);

	int getMatcherSearchRadius() const {return mMatcherSearchRadius;}

	bool trackFrame(std::unique_ptr<Keyframe> frame);
	bool trackFrameHomography(std::unique_ptr<Keyframe> frame);
	bool trackFrame3D(std::unique_ptr<Keyframe> frame);

	void resync();

	bool isLost() const { return mIsLost; }

	const Eigen::Matrix3fr &getCurrentPose() const { return mCurrentPose; }
	void setCurrentPose(const Eigen::Matrix3fr &pose) { mCurrentPose = pose; }

	const Eigen::Vector2i getImageSize() const {return mImageSize;}
	const int getOctaveCount() const {return mOctaveCount;}

	const Keyframe *getFrame() const { return mFrame.get(); }

	//const cv::Matx23f &getFrameToLastSimilarity() const { return mSimilarityInv; }
	//const cv::Matx23f &getLastToFrameSimilarity() const { return mSimilarity; }

	const std::vector<FeatureMatch> &getMatches() const { return mMatches; }
	const std::unordered_map<const Feature*, FeatureMatch*> &getMatchMap() const { return mMatchMap; }
	const FeatureMatch *getMatch(const Feature *feature) const
	{
		auto it = mMatchMap.find(feature);
		return (it == mMatchMap.end()) ? NULL : it->second;
	}

	int getMatchInlierCount() const { return mMatchInlierCount; }
	const std::vector<MatchReprojectionErrors> &getReprojectionErrors() const { return mReprojectionErrors; }

protected:
	/////////////////////////////////////////////////////
	// Protected members

	bool mIsLost;

	Map *mMap;

	Eigen::Vector2i mImageSize;
	int mOctaveCount;

	int mMatcherSearchRadius; //Contrary to the flag, this is in image pixel units

	Eigen::Matrix3fr mCurrentPose; //This is a homography, used before calibration
	Eigen::Matrix3fr mCurrentPoseR; //This is a rotation, used after calibration
	Eigen::Vector3f mCurrentPoseT; //This is a rotation, used after calibration

	//Data from the previous frame
	//Only inliers are kept here
	//std::unique_ptr<FrameTrackingData> mLastTrackedFrameDat;
	std::unique_ptr<Keyframe> mLastFrame;
	std::vector<FeatureMatch> mLastMatches;

	//Data from the current frame
	std::unique_ptr<Keyframe> mFrame;

	std::vector<std::vector<FeatureProjectionInfo>> mFeaturesInView; //Outer vector is of octaves, inner of projections

	std::vector<FeatureMatch> mMatches;
	std::unordered_map<const Feature*, FeatureMatch *> mMatchMap; //Second is pointer into mMatches

	int mMatchInlierCount;
	std::vector<MatchReprojectionErrors> mReprojectionErrors;	//Same index as mMatches

	/////////////////////////////////////////////////////
	// Protected methods

	void findMatches();
};

} /* namespace dtslam */

#endif /* POSETRACKER_H_ */

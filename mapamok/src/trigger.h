class trigger
{
public:
	trigger()
	{
	}
	void setup(float ttime)
	{
		triggerTime = ttime;
		hasTriggered = false;
	}
	bool evaluate(float currentTime)
	{
		if(currentTime > triggerTime && hasTriggered == false)
		{
			hasTriggered = true;
			return true;
		}
		return false;
	}
	void reset()
	{
		hasTriggered = false;
	}
protected:
	float triggerTime;
	bool hasTriggered;
};

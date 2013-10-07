#pragma once

class FormConnect :
	public Window
{
public:
	FormConnect();
	~FormConnect();

	Button butOK_;
	Control textHostName_;
	Control textPSW_;

	void OnInit() override;
};


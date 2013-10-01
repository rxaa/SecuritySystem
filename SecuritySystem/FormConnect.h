#pragma once

class FormConnect :
	public Window
{
public:
	FormConnect();
	~FormConnect();

	Button butOK_;
	Control textHostName_;

	void OnInit() override;
};


#pragma once

class AboutForm :
	public Window
{
public:
	AboutForm(void);
	~AboutForm(void);

	Button butOK_;
	
	void OnInit() override;
};


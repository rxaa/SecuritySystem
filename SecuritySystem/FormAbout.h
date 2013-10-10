#pragma once

class FormAbout :
	public Window
{
public:
	FormAbout(void);
	~FormAbout(void);

	Button butOK_;
	
	void OnInit() override;
};


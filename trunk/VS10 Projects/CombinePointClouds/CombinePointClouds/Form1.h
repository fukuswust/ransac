#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <stdio.h>

using namespace System::Runtime::InteropServices;
using namespace std;
namespace CombinePointClouds {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  button1;
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->AccessibleName = L"newBtn";
			this->button1->Location = System::Drawing::Point(12, 12);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"New";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// Form1
			// 
			this->AccessibleName = L"mainForm";
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 262);
			this->Controls->Add(this->button1);
			this->Name = L"Form1";
			this->Text = L"Combine Point Clouds";
			this->ResumeLayout(false);

		}
#pragma endregion



	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;

			openFileDialog1->InitialDirectory = "c:\\";
			openFileDialog1->Filter = "Depth Files (*.csv)|*.csv|All files (*.*)|*.*";
			openFileDialog1->FilterIndex = 1;
			openFileDialog1->RestoreDirectory = true;

			if ( openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK ) {
				char* filePath = (char*)(void*)Marshal::StringToHGlobalAnsi(openFileDialog1->FileName);
				ifstream infile(filePath);
				if (infile) {
					string inLine;
					int j = 0;
					while ( infile.good() && (j < 480) ) {
						int i = 0;
						getline (infile,inLine);
						vector<string> inPart;
						split(inLine,',',inPart);
						for (i = 0; i < 640; i++) {
							char strInPart[100];
							strcpy(strInPart, inPart[i].c_str());
							float depth = atof(strInPart);

						}
						++j;
					}
					infile.close();
				}
			}
		}
	};
}


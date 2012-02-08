#include "DTIAtlasFiberAnalyzerguiwindow.h"
#include "GlobalFunction.h"

#include <sstream>

/********************************************************************************* 
 * Constructor
 ********************************************************************************/
DTIAtlasFiberAnalyzerguiwindow::DTIAtlasFiberAnalyzerguiwindow(bool debug, QWidget * parent , Qt::WFlags f  ): QMainWindow(parent, f)
{
	setupUi(this);
	
	/* Parameters */
	m_debug = debug;
	m_numberstep = 0;
	m_CSV = NULL;
	m_DataCol = -1;
	m_DeformationCol = -1;
	m_NameCol = -1;
	m_CSVcreated = false;
	m_nextStep = false;
	m_currentRow = -1;
	m_currentColumn = -1;
	m_transposeColRow = false;
	
	nextstep->setEnabled(false);
	previousstep->setEnabled(false);
	Apply->setVisible(false);
	this->setMouseTracking(true);
	
	/* set the main layout */
	/* ScrollArea */
	QScrollArea *const scroll(new QScrollArea);
	centralwidget->setLayout(PrincipalLayout);
	scroll->setWidget(centralwidget);
	setCentralWidget(scroll);
	
	//Design
	CsvTitle->setFont(QFont("Courrier", 20,4));
	Fiberprocesstitle->setFont(QFont("Courrier", 20,4));
	Propertiestitle->setFont(QFont("Courrier", 20,4));
	
	//Disable every tab except the first one
	for(int i=1; i<4; i++)
		PrincipalWidget->setTabEnabled (i,false);
	PrincipalWidget->setTabEnabled(4,true);
	PrincipalWidget->setCurrentIndex(0);
	
	//Load Default Analysis file
	LoadAnalysisFile(CreateDefaultAnalysisFile());
	
	//General Slots
	connect(this->nextstep, SIGNAL(clicked()), this, SLOT(NextStep()));
	connect(this->previousstep, SIGNAL(clicked()), this, SLOT(PreviousStep()));
	connect(this->apply, SIGNAL(clicked()), this, SLOT(applyNewCSV()));
	connect(this->AutomatedComputation, SIGNAL(clicked()), this, SLOT(AutoCompute()));
	connect(this->PrincipalWidget, SIGNAL(currentChanged(int)), this, SLOT(TabChanged(int)));
	
	//Tab 1
	
	connect(this->csvfilename, SIGNAL(editingFinished()), this, SLOT(EnterCsvFileName()));
	connect(this->browsercsvfilename, SIGNAL(clicked()), this, SLOT(BrowserCSVFilename()));	
	connect(this->deletecsv, SIGNAL(clicked()), this, SLOT(ClearDataInformation()));
	connect(this->savecsv, SIGNAL(clicked()), this, SLOT(BrowserSaveCSV()));
	connect(this->addcolumn, SIGNAL(clicked()), this, SLOT(AddC()));
	connect(this->CSVtable, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(SetCurrentColumn()));
	connect(this->deletecolumn, SIGNAL(clicked()), this, SLOT(DeleteC()));
	connect(this->addrow, SIGNAL(clicked()), this, SLOT(AddR()));
	connect(this->CSVtable, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(SetCurrentRow()));
	connect(this->deleterow, SIGNAL(clicked()), this, SLOT(DeleteR()));
	connect(this->ColumnData, SIGNAL(editingFinished()), this, SLOT(SetDataColumn()));
	connect(this->ColumnDeformation, SIGNAL(editingFinished()), this, SLOT(SetDeformationColumn()));
	connect(this->ColumnNameCases, SIGNAL(editingFinished()), this, SLOT(SetNameColumn()));
	connect(this->CSVtable, SIGNAL(cellDoubleClicked( int, int)),this, SLOT(CellAsBrowser(int,int)));
	connect(this->CSVtable->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(ChangeHeader(int)));
	connect(this->browserOutputFolder, SIGNAL(clicked()), this, SLOT(BrowserOutputFolder()));
	connect(this->outputfolder, SIGNAL(editingFinished()), this, SLOT(EnterOutputFolder()));

	/* Tab 2 */
	connect(this->browserAtlasFiberFolder, SIGNAL(clicked()), this, SLOT(BrowserAtlasFiberFolder()));
	connect(this->AtlasFiberFolder, SIGNAL(editingFinished()), this, SLOT(EnteronAtlasFiberFolder()));
	connect(this->Apply, SIGNAL(clicked()), this, SLOT(ApplySlot()));
	connect(this->AddFiber, SIGNAL(clicked()), this, SLOT(AddFiberInList()));
	connect(this->AddAllFiber, SIGNAL(clicked()), this, SLOT(SelectAllFiber()));
	connect(this->RemoveFiber, SIGNAL(clicked()), this, SLOT(RemoveFiberInList()));
	connect(this->Removeallfiber, SIGNAL(clicked()), this, SLOT(RemoveAllFiber()));
	
	/* Tab 3 */
	connect(this->FABox, SIGNAL(stateChanged(int)), this, SLOT(CheckNextStep()));
	connect(this->MDBox, SIGNAL(stateChanged(int)), this, SLOT(CheckNextStep()));
	connect(this->FroBox, SIGNAL(stateChanged(int)), this, SLOT(CheckNextStep()));
	connect(this->l1Box, SIGNAL(stateChanged(int)), this, SLOT(CheckNextStep()));
	connect(this->l2Box, SIGNAL(stateChanged(int)), this, SLOT(CheckNextStep()));
	connect(this->l3Box, SIGNAL(stateChanged(int)), this, SLOT(CheckNextStep()));
	connect(this->RDBox, SIGNAL(stateChanged(int)), this, SLOT(CheckNextStep()));
	connect(this->GABox, SIGNAL(stateChanged(int)), this, SLOT(CheckNextStep()));
	
	/* Tab 4 */
	connect(this->PlotButton, SIGNAL(clicked()), this, SLOT(OpenPlotWindow()));
	
	/* Tab 5 */
	connect(this->DTIPbrowsercsv, SIGNAL(clicked()), this, SLOT(BrowserDTIPCsvFilename()));
	connect(this->DTIPbrowservtk, SIGNAL(clicked()), this, SLOT(BrowserDTIPVtkFilename()));
	connect(this->DTIPbrowseroutput, SIGNAL(clicked()), this, SLOT(BrowserDTIPOutputFilename()));
	connect(this->DTIPcsvfilename, SIGNAL(editingFinished()), this, SLOT(CheckNextStep()));
	connect(this->DTIPvtkfilename, SIGNAL(editingFinished()), this, SLOT(CheckNextStep()));
	connect(this->DTIPoutputfilename, SIGNAL(editingFinished()), this, SLOT(CheckNextStep()));
	connect(this->DTIPcomputes, SIGNAL(clicked()), this, SLOT(ComputeDTIParametrization()));
	
	//"File" action
	connect(this->actionSave_data_file, SIGNAL(triggered()), SLOT(SaveDataAction()));
	connect(this->actionSave_analysis_file, SIGNAL(triggered()), SLOT(SaveAnalysisAction()));
	connect(this->actionOpen_data_file, SIGNAL(triggered()), SLOT(OpenDataFile()));
	connect(this->actionOpen_analysis_file, SIGNAL(triggered()), SLOT(OpenAnalysisFile()));
}

/***************************************************
 * 					General Slots
 ***************************************************/
 
/***************************************************
 *PreviousStep: Go to previous tab.
 ***************************************************/
void DTIAtlasFiberAnalyzerguiwindow::PreviousStep()
{
	//change actual step
	m_numberstep--;
	
	//Modify widget corresponding to current step
	PrincipalWidget->setCurrentWidget(stepwidget(m_numberstep));
	CheckNextStep();
}


/***************************************************
 *NextStep: Go to next tab.
 ***************************************************/
 
void DTIAtlasFiberAnalyzerguiwindow::NextStep()
{
	if(m_numberstep==0)
	{
		//If no filename was entered -> create a new one.
		if(csvfilename->text().compare("")==0)
		{
			int number=0;
			bool nofile = false;
			std::stringstream SNumber;
			SNumber << number;
			m_csvfilename =  m_OutputFolder + "/Database_" + SNumber.str() + ".csv";
			// check if there is already a file with the default name
			while(!nofile)
			{
				if(!FileExisted(m_csvfilename))
					nofile = true;
				else
				{
					std::stringstream SNumber2;
					number = number + 1;
					SNumber2 << number;
					m_csvfilename =  m_OutputFolder + "/Database_" + 
							SNumber2.str() + ".csv";
				}
			}
			m_CSV->SetFilename(m_csvfilename);
			csvfilename->setText(m_csvfilename.c_str());
			SaveCSV();
		}
		//widget modifications
		nextstep->setVisible(false);
		Apply->setVisible(true);
		Apply->setEnabled(false);
	}
	else if(m_numberstep==1)
	{
		//widget modifications
		nextstep->setVisible(false);
		Apply->setVisible(true);
		Apply->setEnabled(false);
	}
	else if(m_numberstep==2)
	{
		//widget modifications
		FillDataFilesList();
		nextstep->setEnabled(false);
	}
	//Go to next tab.
	m_numberstep++;
	PrincipalWidget->setTabEnabled(m_numberstep,true);
	PrincipalWidget->setCurrentWidget(stepwidget(m_numberstep));
	
	//Widget modifications
	CheckNextStep();
}

/*********************************************************************************
 *ApplySlot: Use fiberprocess or dtitractstat depending on which step we are.
 *********************************************************************************/

void DTIAtlasFiberAnalyzerguiwindow::ApplySlot()
{
	//Apply function depends on which step you are.
	if(m_numberstep==1)
		Computefiberprocess();
	else if(m_numberstep==2)
		Computedti_tract_stat();
	
	//After computation we can go to next step
	nextstep->setEnabled(true);
}

/*********************************************************************************
 *TabChanged: Slot used when user changed tab
 *********************************************************************************/

void DTIAtlasFiberAnalyzerguiwindow::TabChanged(int index)
{
	//change current step
	m_numberstep=PrincipalWidget->currentIndex();
	CheckNextStep();
}

/*********************************************************************************
 *CheckNextStep: Check conditions to enable or not the different buttons.
 *********************************************************************************/

void DTIAtlasFiberAnalyzerguiwindow::CheckNextStep()
{
	//Previous always visible
	if(m_numberstep==0)
	{
		//Next visible & disabled; Previous disabled; Apply not visible
		previousstep->setEnabled(false);
		Apply->setVisible(false);
		nextstep->setVisible(true);
		
		//if an output folder, a datacolumn is precised and a csv existed Next becomes enabled
		if(outputfolder->text().compare("")!=0 && m_CSV!=NULL && m_DataCol!=-1)
			nextstep->setEnabled(true);
		else
			nextstep->setEnabled(false);
		AutomatedComputation->setEnabled(true);
	}
	if(m_numberstep==1)
	{
		//Next not visible; Previous enabled; Apply visible & disabled
		previousstep->setEnabled(true);
		Apply->setVisible(true);
		nextstep->setVisible(false);
		//if there is an atlas folder and at least one fiber is selected Apply becomes enabled
		if(m_AtlasFiberDir.size()!=0 && m_FiberSelectedname.size()!=0)
			Apply->setEnabled(true);
		else
			Apply->setEnabled(false);
		AutomatedComputation->setEnabled(true);
	}
	if(m_numberstep==2)
	{
		//Next not visible; Previous enabled; Apply visible & disabled
		previousstep->setEnabled(true);
		Apply->setVisible(true);
		nextstep->setVisible(false);
		setParam();
		//if there is at least one parameter Apply becomes enabled
		if(m_NumberOfParameters==0)
			Apply->setEnabled(false);
		else
			Apply->setEnabled(true);
		AutomatedComputation->setEnabled(true);
	}
	if(m_numberstep==3)
	{
		//Next visible & disabled; Previous enabled; Apply not visible
		previousstep->setEnabled(true);
		Apply->setVisible(false);
		nextstep->setVisible(true);
		nextstep->setEnabled(false);
		AutomatedComputation->setEnabled(true);
		
	}
	if(m_numberstep==4)
	{
		Apply->setVisible(false);
		nextstep->setVisible(true);
		nextstep->setEnabled(false);
		previousstep->setEnabled(false);
		AutomatedComputation->setEnabled(false);
		if(DTIPoutputfilename->text().toStdString().compare("")==0 || !IsFile(DTIPoutputfilename->text().toStdString()) || DTIPcsvfilename->text().toStdString().compare("")==0 || DTIPvtkfilename->text().toStdString().compare("")==0)
			DTIPcomputes->setEnabled(false);
		else
			DTIPcomputes->setEnabled(true);
	}
		
}

/********************************************************************************* 
 * Widget associates to numberstep
 ********************************************************************************/
QWidget* DTIAtlasFiberAnalyzerguiwindow::stepwidget(int numberstep)
{
	switch(numberstep)
	{
		case 0:
			return Step1;
		case 1:
			return Step2;
		case 2:
			return Step3;
		case 3:
			return Step4;
	}
	return NULL;
}

/********************************************************************************* 
 * Computes using parameter file informations
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::AutoCompute()
{	
	if(m_AtlasFiberDir.size()==0)
	{
		QMessageBox::warning(this, "Warning", "No Atlas directory specified!");
		return;
	}
	else if(m_FiberSelectedname.size()==0)
	{
		QMessageBox::warning(this, "Warning", "No Fibers selected!");
		return;
	}
	else if(m_parameters.size()==0)
	{
		QMessageBox::warning(this, "Warning", "No parameters selected!");
		return;
	}
	else if(m_OutputFolder.size()==0)
	{
		QMessageBox::warning(this, "Warning", "No output folder specified!");
		return;
	}
	else if(m_CSV==NULL || m_DataCol==-1)
	{
		QMessageBox::warning(this, "Warning", "No input data or data column specified!");
		return;
	}
	else
	{
		if(m_numberstep<=3)
		{
			if(m_numberstep<=2)
			{
				if(m_numberstep<=1)
				{
					NextStep();
					if(!Computefiberprocess())
						return;
				}
			
				NextStep();
				if(!Computedti_tract_stat())
					return;
			}
		
			NextStep();
			if(!OpenPlotWindow())
				return;
		}
	}
}

/***************************************************
 * 					TAB 1
 ***************************************************/

/* CSV Load */
/********************************************************************************* 
 * Open a dialog to search the name of the vector's path and write it in the GUI
 * Widget
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::BrowserCSVFilename()
{
	/* Mouse Event when running */
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QString filename,type;
	filename = QFileDialog::getOpenFileName(this, "Open CSV File", "/", "Text (*.csv *.txt)",&type);
	csvfilename->setText(filename);
	EnterCsvFileName();
	
	/* Restore the mouse */
	QApplication::restoreOverrideCursor();
}
/********************************************************************************* 
 * When press enter in the QLineEdit in the load frame, load the csv file
 ********************************************************************************/

void DTIAtlasFiberAnalyzerguiwindow::EnterCsvFileName()
{
	/* Mouse Event when running */
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	/* Clear the CSV */
	if(m_CSV !=NULL)
		clearCSV();
	//Clear the number of column
	m_DataCol = -1;
	m_DeformationCol = -1;
	m_NameCol = -1;
	ColumnData->clear();
	ColumnDeformation->clear();
	ColumnNameCases->clear();
	
	if((csvfilename->text().toStdString())!="")
	{
		std::cout<<std::endl;
		std::cout<<"---- New Data ----"<<std::endl;
		
		m_csvfilename = csvfilename->text().toStdString();
		if(!m_CSVcreated)
		{
			m_CSV = new CSVClass(m_csvfilename,m_debug);
			m_CSVcreated = true;
		}
		if(m_CSV->loadCSVFile(m_csvfilename))
		{
			FillCSVFileOnQTable();
			/* Enable the frame to add column and row */
			addCRframe->setEnabled (true);
			saveframe->setEnabled (true);
			ColumnFrame->setEnabled (true);
		}
	}
	CheckNextStep();
	/* Restore the mouse */
	QApplication::restoreOverrideCursor();
}

/********************************************************************************* 
 * FillCSVFileOnQTable: Fill QTable with csv data's informations when enter is pressed
 *  in the QLineEdit in the load frame, load the csv file
 ********************************************************************************/

void DTIAtlasFiberAnalyzerguiwindow::FillCSVFileOnQTable()
{
	// Create the table with the right number of columns and rows
	CSVtable->setRowCount (m_CSV->getRowSize()-1); //minus 1 for the header
	CSVtable->setColumnCount (m_CSV->getColSize(0));
	//Write the name of the header/first line of the csv
	for(unsigned int j = 0; j<m_CSV->getColSize(0);j++)
	{
		//create an item for the QTable
		QTableWidgetItem* header = new QTableWidgetItem;
		QString qs ( ((*m_CSV->getData())[0][j]).c_str() ); //convert the data(string) in QString
		header->setData( 0, qs );
		CSVtable->setHorizontalHeaderItem(j,header);
		//Color the header
		if(j==static_cast<unsigned int>(m_DataCol))
			ColorCol(1);
		if(j==static_cast<unsigned int>(m_DeformationCol))
			ColorCol(2);
		if(j==static_cast<unsigned int>(m_NameCol))
			ColorCol(3);
	}
	
	//fill the tab
	for(unsigned int i = 1; i<m_CSV->getRowSize();i++)
	{
		for(unsigned int j = 0; j<m_CSV->getColSize(i);j++)
		{
			//create an item for the QTable
			QTableWidgetItem* item = new QTableWidgetItem;
			QString qs ( ((*m_CSV->getData())[i][j]).c_str() ); //convert the data(string) in QString
			item->setData( 0, qs );
			CSVtable->setItem (i-1,j,item);
		}
		
	}
}

/* CSV NEW */
/********************************************************************************* 
 * Apply in CSV frame : create a new csv
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::applyNewCSV()
{
	//Clear the number of column
	m_DataCol = -1;
	m_DeformationCol = -1;
	m_NameCol = -1;
	ColumnData->clear();
	ColumnDeformation->clear();
	ColumnNameCases->clear();
	/* Clear the CSV */
	if(m_CSV !=NULL)
		clearCSV();
	//clear the lineEdit
	csvfilename->clear();
	std::cout<<std::endl;
	std::cout<<"---- New Data ----"<<std::endl;
	//add the number of columns
	CSVtable->setColumnCount (numbercolumn->value());
	//add the number of rows
	CSVtable->setRowCount (numberdata->value());
	
	/* Create the CSV class object */
	if(!m_CSVcreated)
	{
		m_CSV = new CSVClass("",m_debug);
		m_CSVcreated = true;
	}
	
	/* Enable the frame to add column and row */
	addCRframe->setEnabled (true);
	saveframe->setEnabled (true);
	ColumnFrame->setEnabled (true);
	m_nextStep = true;
	
	//Fill the Table with empty items
	for(int i=0;i<CSVtable->rowCount();i++)
	{
		for(int j=0;j<CSVtable->columnCount();j++)
		{
			QTableWidgetItem* item = new QTableWidgetItem;
			QString qs (""); //convert the word(string) in QString
			item->setData( 0, qs);
			CSVtable->setItem (i,j,item);
		}
	}
	//widget modifications
	CheckNextStep();
}

/*********************************************************************************
 * ClearDataInformation: Clear all informations concerning data.
 *********************************************************************************/

void DTIAtlasFiberAnalyzerguiwindow::ClearDataInformation()
{
	if(m_CSV!=NULL)
	{
		clearCSV();
		m_CSV=NULL;
	}
	m_CSVcreated = false;
	csvfilename->clear();
	/* Disable the frame to add column and row */
	addCRframe->setEnabled (false);
	saveframe->setEnabled (false);
	ColumnFrame->setEnabled (false);
	//Unable next step
	m_nextStep = false;
	nextstep->setEnabled (true);
	previousstep->setEnabled (false);
	//Reinitialize the current cell to nothing
	m_currentRow = -1;
	m_currentColumn = -1;
	m_numberstep = 0;
	//Disable every tab except the first one
	for(int i=1; i<4; i++)
		PrincipalWidget->setTabEnabled (m_numberstep + i,false);
	m_DataCol = -1;
	m_DeformationCol = -1;
	m_NameCol = -1;
	m_OutputFolder.clear();
	ColumnData->clear();
	ColumnDeformation->clear();
	ColumnNameCases->clear();
	outputfolder->clear();
	CheckNextStep();
}

/* Action on data and QTable */
/********************************************************************************* 
 * Transform the Hearder's names on a std::string
 ********************************************************************************/
std::string DTIAtlasFiberAnalyzerguiwindow::HeaderLine()
{
	QString qs;
	std::string line;
	/* Copy each item in the std::string separate by a coma */
	for(int i=0;i<CSVtable->columnCount();i++)
	{
		if(i!= CSVtable->columnCount() -1)
		{
			if(CSVtable->horizontalHeaderItem(i)!= 0)
			{
				qs = (CSVtable->horizontalHeaderItem(i))->text();
				line+= qs.toStdString();
			}
			line+= ",";
		}
		else
		{
			if(CSVtable->horizontalHeaderItem(i)!= 0)
			{
				qs = (CSVtable->horizontalHeaderItem(i))->text();
				line+= qs.toStdString();
			}
		}
	}
	return line;
}
/********************************************************************************* 
 * Transform the QTable item from a row on a std::string
 ********************************************************************************/
std::string DTIAtlasFiberAnalyzerguiwindow::LineItemsToString(int row)
{
	QString qs;
	std::string line;
	/* Copy each item in the std::string separate by a coma */
	for(int i=0;i<CSVtable->columnCount();i++)
	{
		if(i!= CSVtable->columnCount() -1)
		{
			if(CSVtable->item(row,i)!= 0)
			{
				qs = (CSVtable->item(row,i))->text();
				line+= qs.toStdString();
			}
			else
				line+= "no";
			// Put the coma
			line+= ",";
		}
		else
		{
			if(CSVtable->item(row,i)!= 0)
			{
				qs = (CSVtable->item(row,i))->text();
				line+= qs.toStdString();
			}
			else
				line+= "no";
		}
	}
	return line;
}
/********************************************************************************* 
 * Put the word in the cell (row,column)
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::PutDataonQTable(int row, int column, std::string word)
{
	//create an item for the QTable
	QTableWidgetItem* item = new QTableWidgetItem;
	QString qs ( word.c_str() ); //convert the word(string) in QString
	item->setData( 0, qs );
	CSVtable->setItem (row,column,item);
}


/********************************************************************************* 
 * Delete / Save a CSV file
 ********************************************************************************/
 
void DTIAtlasFiberAnalyzerguiwindow::SaveCSV()
{
	AddDataFromTableToCSV();
	m_CSV->SaveFile(m_csvfilename);
}
void DTIAtlasFiberAnalyzerguiwindow::AutoSaveCSV()
{
	std::string filename;
	filename = takeoffPath(m_csvfilename);
	filename = takeoffExtension(filename);
	filename = m_OutputFolder+ "/" + filename + "_computed.csv";
	AddDataFromTableToCSV();
	m_CSV->SaveFile(filename);
}
void DTIAtlasFiberAnalyzerguiwindow::AddDataFromTableToCSV()
{
	//clear the vector in CSVClass
	m_CSV->clearData();
	//add Header's names
	m_CSV->LineInVector(HeaderLine());
	for(int i=0;i<CSVtable->rowCount();i++)
	{
		//add every line to the vector
		m_CSV->LineInVector(LineItemsToString(i));
	}
}
void DTIAtlasFiberAnalyzerguiwindow::BrowserSaveCSV()
{
	QString output;
	if(m_OutputFolder.size()!=0)
		output=m_OutputFolder.c_str();
	else
		output=".";
	QString file = QFileDialog::getSaveFileName(this, "Save a file", output, "Text (*.csv)");
	
	if(file!=NULL)
	{
		m_csvfilename = file.toStdString();
		m_CSV->SetFilename(m_csvfilename);
		csvfilename->setText(file);
		SaveCSV();
	}
	else
		std::cout<<"ERROR: File name null!"<<std::endl;
}
void DTIAtlasFiberAnalyzerguiwindow::clearCSV()
{
	/* Clear Data in the CSV object */
	m_CSV->clearData();
	//clear table
	CSVtable->clear();
	CSVtable->setColumnCount (1);
	CSVtable->setRowCount (1);
}

/* Row/Column */
/********************************************************************************* 
 * Add collumn
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::AddC()
{
	int columns = CSVtable->columnCount ();
	CSVtable->setColumnCount (columns + 1);
	//add the header
	QTableWidgetItem *header=  new QTableWidgetItem;
	header->setData( 0, headername->text() );
	CSVtable->setHorizontalHeaderItem(columns,header);
	//Clear the Line edit
	headername->clear();
}
void DTIAtlasFiberAnalyzerguiwindow::AddColumnWithHearderName(std::string header)
{
	int columns = CSVtable->columnCount ();
	CSVtable->setColumnCount (columns + 1);
	//add the header
	QTableWidgetItem *title=  new QTableWidgetItem;
	title->setData( 0, header.c_str() );
	CSVtable->setHorizontalHeaderItem(columns,title);
}
/********************************************************************************* 
 * Delete current column
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::DeleteC()
{
	if(m_currentColumn != -1)
	{
		/* Delete the column */
		CSVtable->removeColumn (m_currentColumn);
		m_currentRow = -1;
		m_currentColumn = -1;
	}
	else
		QMessageBox::information(this, "Warning", "Select a column!");
}

/********************************************************************************* 
 * Add row
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::AddR()
{
	int rows = CSVtable->rowCount ();
	CSVtable->setRowCount (rows + 1);
	//fill the data column with no data and deformation field with no deformation
	if(m_DataCol !=-1)
	{
		QTableWidgetItem *item=  new QTableWidgetItem;
		QString str("no data");
		item->setData( 0, str);
		CSVtable->setItem(CSVtable->rowCount()-1,m_DataCol,item);
	}
	if(m_DeformationCol !=-1)
	{
		QTableWidgetItem *item=  new QTableWidgetItem;
		QString str("no deformation");
		item->setData( 0, str);
		CSVtable->setItem(CSVtable->rowCount()-1,m_DeformationCol,item);
	}
	if(m_NameCol !=-1)
	{
		QTableWidgetItem *item=  new QTableWidgetItem;
		QString str("no name");
		item->setData( 0, str);
		CSVtable->setItem(CSVtable->rowCount()-1,m_NameCol,item);
	}
}
/********************************************************************************* 
 * Delete current row
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::DeleteR()
{
	if(m_currentRow != -1)
	{
		/* Delete the row */
		CSVtable->removeRow (m_currentRow);
		m_currentRow = -1;
		m_currentColumn = -1;
	}
	else
		QMessageBox::information(this, "Warning", "Select a line!");
}

/* Set Data column number */
void DTIAtlasFiberAnalyzerguiwindow::SetDataColumn()
{
	if(ColumnData->text().toInt() - 1<CSVtable->columnCount() && ColumnData->text().toInt()!=0
		  && ColumnData->text().toInt()>0)
	{
		// decolor the last column
		DecolorCol(1);
		m_DataCol = ColumnData->text().toInt() -1;
		//check if it was an other column with this number
		if(m_DataCol==m_DeformationCol)
		{
			m_DeformationCol = -1;
			ColumnDeformation->setText("");
			DecolorCol(2);
		}
		else if(m_DataCol==m_NameCol)
		{
			m_NameCol = -1;
			ColumnNameCases->setText("");
			DecolorCol(3);
		}
		ColorCol(1);
		//Fill it with "no data" if there is no data
		for(int i=0;i<CSVtable->rowCount();i++)
		{
			if((CSVtable->item(i,m_DataCol))->text().compare("")==0 ||
						 (CSVtable->item(i,m_DataCol))->text().compare("no")==0)
			{
				QTableWidgetItem *item=  new QTableWidgetItem;
				QString str("no data");
				item->setData( 0, str);
				CSVtable->setItem(i,m_DataCol,item);
			}
		}
	}
	else if((ColumnData->text()).compare("")==0)
	{
		DecolorCol(1);
		m_DataCol = -1;
	}
	else
		ColumnData->setText("");
	CheckNextStep();
}

/* Set Deformation column number */
void DTIAtlasFiberAnalyzerguiwindow::SetDeformationColumn()
{
	if(ColumnDeformation->text().toInt() - 1<CSVtable->columnCount()&& ColumnDeformation->text().toInt()!=0
		  && ColumnDeformation->text().toInt()>0)
	{
		// decolor the last column
		DecolorCol(2);
		m_DeformationCol = ColumnDeformation->text().toInt() -1;
		//check if it was an other column with this number
		if(m_DeformationCol==m_DataCol)
		{
			m_DataCol = -1;
			ColumnData->setText("");
			DecolorCol(1);
		}
		else if(m_DeformationCol==m_NameCol)
		{
			m_NameCol = -1;
			ColumnNameCases->setText("");
			DecolorCol(3);
		}
		ColorCol(2);
		//Fill it with "no deformation" if there is no data
		for(int i=0;i<CSVtable->rowCount();i++)
		{
			if((CSVtable->item(i,m_DeformationCol))->text().compare("")==0 ||
						 (CSVtable->item(i,m_DeformationCol))->text().compare("no")==0)
			{
				QTableWidgetItem *item=  new QTableWidgetItem;
				QString str("no deformation");
				item->setData( 0, str);
				CSVtable->setItem(i,m_DeformationCol,item);
			}
		}
	}
	else if((ColumnDeformation->text()).compare("")==0)
	{
		DecolorCol(2);
		m_DeformationCol = -1;
	}
	else
		ColumnDeformation->setText("");
}

/* Set Name for cases column number */
void DTIAtlasFiberAnalyzerguiwindow::SetNameColumn()
{
	if(ColumnNameCases->text().toInt() - 1 < CSVtable->columnCount() && ColumnNameCases->text().toInt()!=0
		  && ColumnNameCases->text().toInt()>0)
	{
		// decolor the last column
		DecolorCol(3);
		m_NameCol = ColumnNameCases->text().toInt() -1;
		//check if it was an other column with this number
		if(m_NameCol==m_DeformationCol)
		{
			m_DeformationCol = -1;
			ColumnDeformation->setText("");
			DecolorCol(2);
		}
		else if(m_NameCol==m_DataCol)
		{
			m_DataCol = -1;
			ColumnData->setText("");
			DecolorCol(1);
		}
		ColorCol(3);
		//Fill it with "no deformation" if there is no data
		for(int i=0;i<CSVtable->rowCount();i++)
		{
			if((CSVtable->item(i,m_NameCol))->text().compare("")==0 ||
						 (CSVtable->item(i,m_NameCol))->text().compare("no")==0)
			{
				QTableWidgetItem *item=  new QTableWidgetItem;
				QString str("no name");
				item->setData( 0, str);
				CSVtable->setItem(i,m_NameCol,item);
			}
		}
	}
	else if((ColumnNameCases->text()).compare("")==0)
	{
		DecolorCol(3);
		m_NameCol = -1;
	}
	else
		ColumnNameCases->setText("");
}

/* Color the column : 1->Data(green),2->Def(blue),3->Name(orange) */
void DTIAtlasFiberAnalyzerguiwindow::ColorCol(int type)
{
	switch(type)
	{
		case 1:
			if(m_DataCol>=0 && m_DataCol < CSVtable->columnCount())
				paintForeGround(0,147,0,m_DataCol,1);
			break;
		case 2:
			if(m_DeformationCol>=0 && m_DeformationCol < CSVtable->columnCount())
				paintForeGround(85,170,255,m_DeformationCol,2);
			break;
		case 3:
			if(m_NameCol>=0 && m_NameCol < CSVtable->columnCount())
				paintForeGround(255,170,0,m_NameCol,3);
			break;
	}
}

/* Decolor the column : 1->Data(green),2->Def(blue),3->Name(orange) */
void DTIAtlasFiberAnalyzerguiwindow::DecolorCol(int type)
{
	switch(type)
	{
		case 1:
			if(m_DataCol!=-1)
				paintForeGround(0,0,0,m_DataCol,1);
			break;
		case 2:
			if(m_DeformationCol!=-1)
				paintForeGround(0,0,0,m_DeformationCol,2);
			break;
		case 3:
			if(m_NameCol!=-1)
				paintForeGround(0,0,0,m_NameCol,3);
			break;
	}
}

/* Paint the foreground with RGB value */
void DTIAtlasFiberAnalyzerguiwindow::paintForeGround(int R,int G,int B,int col,int type)
{
	QString headername;
	QTableWidgetItem *header=  new QTableWidgetItem;
	if(CSVtable->horizontalHeaderItem(col)!=0)
	{
		headername = CSVtable->horizontalHeaderItem(col)->text();
	}
	else
	{
		switch(type)
		{
			case 1:
				headername = "Data";
				break;
			case 2:
				headername = "Deformation field";
				break;
			case 3:
				headername = "Name";
				break;
		}
	}
	header->setData( 0, headername);
	QBrush brush(QColor(R,G,B,255));
	header->setForeground(brush);
	CSVtable->setHorizontalHeaderItem(col,header);
}


/* Set Current row */
void DTIAtlasFiberAnalyzerguiwindow::SetCurrentRow()
{
	m_currentRow = CSVtable->currentRow();
}

/* Set Current Column */
void DTIAtlasFiberAnalyzerguiwindow::SetCurrentColumn()
{
	m_currentColumn = CSVtable->currentColumn();
}

/* Change the header of a column */
void DTIAtlasFiberAnalyzerguiwindow::ChangeHeader(int col)
{
	bool ok;
	//New header
	QTableWidgetItem *header=  new QTableWidgetItem;
	//ask the name
	QString headername = QInputDialog::getText(this,"header", "name of the column :", QLineEdit::Normal,
															 QString::null, &ok);
	if(ok && !headername.isEmpty())
	{
		header->setData( 0, headername );
		CSVtable->setHorizontalHeaderItem(col,header);
		if(col == m_DataCol)
			ColorCol(1);
		if(col == m_DeformationCol)
			ColorCol(2);
		if(col == m_NameCol)
			ColorCol(3);
	}
}

/* Define the cell in the QTableWidget like a browser */
void DTIAtlasFiberAnalyzerguiwindow::CellAsBrowser(int row,int col)
{
	if(col==m_DataCol)
		BrowserIndividualData(row);
	if(col==m_DeformationCol)
		BrowserDeformationField(row);
}

/* Set Param */
void DTIAtlasFiberAnalyzerguiwindow::setParam()
{
	m_parameters = "";
	m_NumberOfParameters = 0 ;
	//FA
	if(FABox->isChecked())
	{
		m_parameters+="fa,";
		m_NumberOfParameters++;
	}
	//MD
	if(MDBox->isChecked())
	{
		m_parameters+="md,";
		m_NumberOfParameters++;
	}
	//Fro
	if(FroBox->isChecked())
	{
		m_parameters+="fro,";
		m_NumberOfParameters++;
	}
	//lambda 1
	if(l1Box->isChecked())
	{
		m_parameters+="ad,";
		m_NumberOfParameters++;
	}
	//lambda 2
	if(l2Box->isChecked())
	{
		m_parameters+="l2,";
		m_NumberOfParameters++;
	}
	//lambda 3
	if(l3Box->isChecked())
	{
		m_parameters+="l3,";
		m_NumberOfParameters++;
	}
	//RD
	if(RDBox->isChecked())
	{
		m_parameters+="rd,";
		m_NumberOfParameters++;
	}
	//GA
	if(GABox->isChecked())
	{
		m_parameters+="ga,";
		m_NumberOfParameters++;
	}
	
	if(m_parameters.compare("")!=0)
		m_parameters=m_parameters.substr(0,m_parameters.find_last_of(","));
}


/* Load Data/Deformation in the CSV Cell */
/********************************************************************************* 
 * load files for Individual images and put them in the Qtable
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::BrowserIndividualData(int row)
{
	QString filename,type;
	filename = QFileDialog::getOpenFileName(this, "Open Individual Data", "/", 
														 "Images (*.gipl *.hdr *.nhdr *.nrrd)",&type);
	
	if(m_debug)
		std::cout<<"Filename : "<< (filename.toStdString()).c_str() <<std::endl;
	
	/* Keep the value if cancel */
	if(filename !=NULL)
	{
		PutDataonQTable(row,m_DataCol,filename.toStdString());
	}
	else if(m_debug)
		std::cout<<"Error : filename is NULL."<<std::endl;
}
/********************************************************************************* 
 * load file for deformation field and put it in the Qtable
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::BrowserDeformationField(int row)
{
	QString filename,type;
	filename = QFileDialog::getOpenFileName(this, "Open Deformation Field", "/");
	
	if(m_debug)
		std::cout<<"Filename : "<< (filename.toStdString()).c_str() <<std::endl;
	
	/* Keep the value if cancel */
	if(filename !=NULL)
	{
		PutDataonQTable(row,m_DeformationCol,filename.toStdString());
	}
	else if(m_debug)
		std::cout<<"Error : filename is NULL."<<std::endl;
}

/********************************************************************************* 
 * Browser the output folder
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::BrowserOutputFolder()
{
	QString path = QFileDialog::getExistingDirectory(this);
	if(path!=NULL)
	{
		outputfolder->setText(path);
		EnterOutputFolder();
	}
}
/********************************************************************************* 
 * change the output folder when enter in the lineEdit
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::EnterOutputFolder()
{
	m_OutputFolder=(outputfolder->text()).toStdString();
	CheckNextStep();
}


/***************************************************
 * 					TAB 2
 ***************************************************/
 
/********************************************************************************* 
 * Find the atlas directory with a browser window
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::BrowserAtlasFiberFolder()
{
	QString path = QFileDialog::getExistingDirectory(this);
	if(path!=NULL)
	{
		AtlasFiberFolder->setText(path);
		EnteronAtlasFiberFolder();
	}
}
/********************************************************************************* 
* when you finish editing the atlas directory
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::EnteronAtlasFiberFolder()
{
	if(AtlasFiberFolder->text().compare("")!=0)
	{
		//Clear the vectors of Fiber's names and the Lists
		m_Fibername.clear();
		m_FiberSelectedname.clear();
		m_Fibersplane.clear();
		m_RelevantPlane.clear();
		//clear the list
		FiberInAtlasList->clear ();
		FiberSelectedList->clear ();
		FiberSelected2List->clear ();
		FiberPlaneFile->clear ();
		//Set the path and Fill the list with the fiber
		m_AtlasFiberDir=AtlasFiberFolder->text().toStdString();
		FillFiberFrame();
	}
	else
	{
		//Clear the vectors of Fiber's names and the Lists
		m_Fibername.clear();
		m_FiberSelectedname.clear();
		m_Fibersplane.clear();
		m_RelevantPlane.clear();
		//clear the list
		FiberInAtlasList->clear ();
		FiberSelectedList->clear ();
		FiberSelected2List->clear ();
		FiberPlaneFile->clear ();
		m_nextStep = false;
	}
	CheckNextStep();
}


/********************************************************************************* 
 * Fill the fiber frame with the name of the fiber form the atlas directory
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::FillFiberFrame()
{
	ReadFiberNameInAtlasDirectory(m_Fibername,m_Fibersplane, m_AtlasFiberDir);
	
	//Print the name in the ListWidget
	for( unsigned int j=0;j<m_Fibername.size();j++)
		FiberInAtlasList->addItem(m_Fibername[j].c_str());
}

/********************************************************************************* 
 * Add the fiber selected into the list
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::AddFiberInList()
{
	if(m_debug)
		std::cout<<"Add Fibers"<<std::endl;
	QList<QListWidgetItem *> SelectedFiber;
	SelectedFiber = FiberInAtlasList->selectedItems();
	std::vector<int> plane;
	
	//add this in the FiberSelectedList and in the vector
	for(int i=0;i<SelectedFiber.count();i++)
	{
		//Remove and add the item in the List and vector
		for(int j=0;j<FiberInAtlasList->count();j++)
		{
			if(((FiberInAtlasList->item(j))->text()).compare((SelectedFiber.at(i))->text())==0)
			{
				QListWidgetItem* item = new QListWidgetItem;
				QString qs((SelectedFiber.at(i))->text());
				item->setData( 0, qs );
				FiberSelectedList->addItem(FiberInAtlasList->takeItem(j));
				FiberSelected2List->addItem(item);
				m_FiberSelectedname.push_back(((SelectedFiber.at(i))->text()).toStdString());
				/* Check the plane and add/erase it */
				plane = PlaneAssociatedToFiber(((SelectedFiber.at(i))->text()).toStdString(), m_Fibersplane);
				if(plane.size()!=0)
				{
					for(unsigned int k=0; k<plane.size(); k++)
					{
						m_RelevantPlane.push_back(m_Fibersplane[plane[k]]);
						QListWidgetItem* itemplane= new QListWidgetItem;
						QString qs(m_Fibersplane[plane[k]].c_str());
						itemplane->setData( 0, qs );
						FiberPlaneFile->addItem(itemplane);
						m_Fibersplane.erase(m_Fibersplane.begin()+plane[k]);
					}
				}
			}
		}
		
		//remove the name in the first vector
		for(unsigned int k=0;k<m_Fibername.size();k++)
		{
			if(m_Fibername[k].compare(((SelectedFiber.at(i))->text()).toStdString())==0)
				m_Fibername.erase(m_Fibername.begin()+k);
		}
	}
	for(int i=0; i<FiberPlaneFile->count(); i++)
		FiberPlaneFile->item(i)->setSelected(true);
	CheckNextStep();
}

/********************************************************************************* 
 * Add all fibers into the list
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::SelectAllFiber()
{
	int found;
	std::vector<int> plane;
	//Read all the fibername data
	for(int i=0;i<FiberInAtlasList->count();i++)
	{
		found=-1;
		for( unsigned int j=0;j<m_Fibername.size();j++)
		{
			if(((FiberInAtlasList->item(i))->text()).compare(m_Fibername[j].c_str())==0)
			{
				found=j;
				break;
			}
		}
		if(found>=0)
		{
			QListWidgetItem* item= new QListWidgetItem;
			item=(FiberInAtlasList->takeItem(i));
			i--;
			FiberSelectedList->addItem(item);
			FiberSelected2List->addItem(item);
			m_FiberSelectedname.push_back(item->text().toStdString());
			plane = PlaneAssociatedToFiber(m_FiberSelectedname[m_FiberSelectedname.size()-1], m_Fibersplane);
			if(plane.size()!=0)
			{
				for(unsigned int k=0; k<plane.size(); k++)
				{
					m_RelevantPlane.push_back(m_Fibersplane[plane[k]]);
					QListWidgetItem* itemplane= new QListWidgetItem;
					QString qs(m_Fibersplane[plane[k]].c_str());
					itemplane->setData( 0, qs );
					FiberPlaneFile->addItem(itemplane);
					m_Fibersplane.erase(m_Fibersplane.begin()+plane[k]);
				}
			}			
		}
	}
	
	//Read all the fiberplane name data
	for( unsigned int i=0;i<m_Fibersplane.size();i++)
	{
		QListWidgetItem* itemplane= new QListWidgetItem;
		QString qs(m_Fibersplane[i].c_str());
		itemplane->setData( 0, qs );
		FiberPlaneFile->addItem(itemplane);
	}
	//Clear the vector with the initial list of fibers' name.
	m_Fibername.clear();
	m_Fibersplane.clear();
	CheckNextStep();
}



/********************************************************************************* 
 * Remove the fiber selected from the list
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::RemoveFiberInList()
{
	if(m_debug)
		std::cout<<"Remove Fibers"<<std::endl;
	QList<QListWidgetItem *> SelectedFiber;
	SelectedFiber = FiberSelectedList->selectedItems();
	std::vector<int> plane;
	
	//remove this from the FiberSelectedList and from the vector
	for(int i=0;i<SelectedFiber.count();i++)
	{
		//Remove the item
		for(int j=0;j<FiberSelectedList->count();j++)
		{
			if(((FiberSelectedList->item(j))->text()).compare((SelectedFiber.at(i))->text())==0)
			{
				FiberInAtlasList->addItem(FiberSelectedList->takeItem(j));
				FiberSelected2List->takeItem(j);
				m_Fibername.push_back(((SelectedFiber.at(i))->text()).toStdString());
				
				/* Plane */
				plane = PlaneAssociatedToFiber(((SelectedFiber.at(i))->text()).toStdString(), m_RelevantPlane);
				if(plane.size()!=0)
				{
					for(unsigned int k=0; k<plane.size(); k++)
					{
						FiberPlaneFile->takeItem(plane[k]);
						m_Fibersplane.push_back(m_RelevantPlane[plane[k]]);
						m_RelevantPlane.erase(m_RelevantPlane.begin()+plane[k]);
					}
				}
			}
		}
		
		//remove the name in the vector
		for(unsigned int k=0;k<m_FiberSelectedname.size();k++)
		{
			if(m_FiberSelectedname[k].compare(((SelectedFiber.at(i))->text()).toStdString())==0)
				m_FiberSelectedname.erase(m_FiberSelectedname.begin()+k);
		}
	}
	CheckNextStep();
}



/********************************************************************************* 
 * Remove all fibers into the list
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::RemoveAllFiber()
{
	//Read all the fibername data
	for( unsigned int i=0;i<m_FiberSelectedname.size();i++)
	{
		for(int j=0;j<FiberSelectedList->count();j++)
		{
			if(((FiberSelectedList->item(j))->text()).compare(m_FiberSelectedname[i].c_str())==0)
			{
				FiberSelectedList->takeItem(j);
				FiberSelected2List->takeItem(j);
			}
		}
		
		QListWidgetItem* item= new QListWidgetItem;
		QString qs(m_FiberSelectedname[i].c_str());
		item->setData( 0, qs );
		FiberInAtlasList->addItem(item);
		m_Fibername.push_back(m_FiberSelectedname[i]);
	}
	
	//Read all the fiberplanename data
	for( unsigned int i=0;i<m_RelevantPlane.size();i++)
	{
		for(int j=0;j<FiberPlaneFile->count();j++)
		{
			if(((FiberPlaneFile->item(j))->text()).compare(m_RelevantPlane[i].c_str())==0)
				FiberPlaneFile->takeItem(j);
		}
		
		m_Fibersplane.push_back(m_RelevantPlane[i]);
	}
	
	//Clear the vector with the initial list of fibers' name.
	m_FiberSelectedname.clear();
	m_RelevantPlane.clear();
	CheckNextStep();
}



/*********************************************************************************
 * Compute fiberprocess
 ********************************************************************************/
bool DTIAtlasFiberAnalyzerguiwindow::Computefiberprocess()
{
	int ret=QMessageBox::Cancel;
	/* Mouse Event when running */
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	/* Call of fiberprocess if there is a Data/Deformation columns and at least one fiber */
	if(m_DataCol==-1)
		QMessageBox::information(this,"Warning","No Data column set, you have to set one!");
	else if(m_FiberSelectedname.empty())
		QMessageBox::information(this,"Warning","No fiber selected!");
	else
	{
		std::string pathFiberProcess;
		//Add the data to the CSV
		AddDataFromTableToCSV();
		
		//Find path for fiberprocess
		pathFiberProcess= itksys::SystemTools::FindProgram("fiberprocess");
		//if path not found
		if(pathFiberProcess.empty()==true)
		{
			ret = QMessageBox::information(this, "fiberprocess","Select the folder where fiberprocess* is saved .");
			pathFiberProcess = (QFileDialog::getExistingDirectory(this)).toStdString();
			pathFiberProcess = pathFiberProcess +"/fiberprocess";
		}
		
		if(pathFiberProcess.compare("")!=0)
		{
			//Apply fiberprocess on CSV data
			if(!Applyfiberprocess(m_CSV, pathFiberProcess, m_AtlasFiberDir, m_OutputFolder,m_DataCol, m_DeformationCol, m_NameCol, m_FiberSelectedname, m_parameters,m_transposeColRow, false, this))
			{
				std::cout<<"fiberprocess has been canceled"<<std::endl;
				QApplication::restoreOverrideCursor();
				return false;
			}
			else
			{
				nextstep->setVisible(true);
				Apply->setVisible(false);
				//Add the data to the Table
				FillCSVFileOnQTable();
			}
		}
	}
	
	/* Restore the mouse */
	QApplication::restoreOverrideCursor();
	return true;
}



/***************************************************
 * 					TAB 3
 ***************************************************/
/*********************************************************************************
 * Compute dti_tract_stat
 ********************************************************************************/
bool DTIAtlasFiberAnalyzerguiwindow::Computedti_tract_stat()
{
	/* Mouse Event when running */
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	/* Call of fiberprocess if there is a Data/Deformation columns and at least one fiber */
	bool success=false;
	bool CoG=GetAutoPlaneOption();
	std::string pathdti_tract_stat;
	//get the properties
	setParam();
	FillSelectedPlane();
	std::cout<<"selected plane:"<<std::endl;
	for(unsigned int i=0; i<m_SelectedPlane.size(); i++)
		std::cout<<m_SelectedPlane[i]<<std::endl;
	if(m_parameters.compare("")==0)
		QMessageBox::information(this,"Warning","No properties selected!");
	else
	{
		//set the value of the transposition of row and column
		if(ArcOnCase->isChecked())
			m_transposeColRow = false;
		else if(CaseOnArc->isChecked())
			m_transposeColRow = true;
		
		//Add the data to the CSV
		AddDataFromTableToCSV();
		//Find path for fiberprocess
		pathdti_tract_stat= itksys::SystemTools::FindProgram("dtitractstatCLP");
		//if path not found
		std::cout<<pathdti_tract_stat<<std::endl;
		if(pathdti_tract_stat.empty()==true)
		{
			QMessageBox::information(this, "dtitractstatCLP", "Select the folder where dti_tract_stat is located .");
			pathdti_tract_stat = (QFileDialog::getExistingDirectory(this)).toStdString();
			pathdti_tract_stat = pathdti_tract_stat +"/dtitractstatCLP";
		}
		if(pathdti_tract_stat.compare("")!=0)
		{
			//Apply dti tract stat on CSV data
			if(!Applydti_tract_stat(m_CSV, pathdti_tract_stat, m_AtlasFiberDir, m_OutputFolder, m_FiberSelectedname, m_SelectedPlane,m_parameters, m_DataCol, m_DeformationCol, m_NameCol, m_transposeColRow, false, CoG, this))
			{
				std::cout<<"dtitractstat has been canceled"<<std::endl;
				QApplication::restoreOverrideCursor();
				return false;
			}
			else
			{
				setFibers();
				m_FiberProfile=GatheringFiberProfile(m_CSV, m_OutputFolder, m_DataCol, m_NameCol, m_transposeColRow, m_FiberSelectedname,success);
				if(!success||m_FiberProfile.size()<0)
				{
					QMessageBox::warning(this, "Warning", "Error gathering fiber profile\r Please recompute dtitractstat.");
					QApplication::restoreOverrideCursor();
					return false;
				}
				else
				{
					nextstep->setVisible(true);
					Apply->setVisible(false);
				}
			}
			//Add the data to the Table
			FillCSVFileOnQTable();
		}
	}
	/* Restore the mouse */
	QApplication::restoreOverrideCursor();
	return true;
}

void DTIAtlasFiberAnalyzerguiwindow::FillSelectedPlane()
{
	bool found=false;
	std::string fiber, plane;
	QList<QListWidgetItem*> SelectedItems;
	SelectedItems=FiberPlaneFile->selectedItems();
	m_SelectedPlane.clear();
	for(unsigned int i=0; i<m_FiberSelectedname.size(); i++)
	{
		fiber=takeoffExtension(m_FiberSelectedname[i]);
		for(int j=0; j<SelectedItems.size(); j++)
		{
			plane=takeoffExtension(SelectedItems[j]->text().toStdString());
			if(fiber.rfind(plane)!=std::string::npos || plane.rfind(fiber)!=std::string::npos)
			{
				m_SelectedPlane.push_back(SelectedItems[j]->text().toStdString());
				found=true;
				break;
			}
		}
		if(!found)
			m_SelectedPlane.push_back("");
		found=false;
	}
}

/********************************************************************************* 
 * Check the box of profiles with the string parameters (FA, MD, ...)
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::checkBoxProfile(std::string parameters)
{
	//Iterator on the string line
	std::string::iterator it;
	std::string word;
	it = parameters.begin();
	vstring ProfileParam;
	//read the value between two ","
	LineInVector(parameters, ProfileParam);
	//Unchecked all the box
	FABox->setCheckState(Qt::Unchecked);
	MDBox->setCheckState(Qt::Unchecked);
	FroBox->setCheckState(Qt::Unchecked);
	l1Box->setCheckState(Qt::Unchecked);
	l2Box->setCheckState(Qt::Unchecked);
	l3Box->setCheckState(Qt::Unchecked);
	RDBox->setCheckState(Qt::Unchecked);
	GABox->setCheckState(Qt::Unchecked);
	
	//Check it if there is the same name in the vstring
	for(unsigned int i=0;i<ProfileParam.size();i++)
	{
		if(ProfileParam[i].compare("fa")==0)
			FABox->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("md")==0)
			MDBox->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("fro")==0)
			FroBox->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("l1")==0 || ProfileParam[i].compare("ad")==0)
			l1Box->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("l2")==0)
			l2Box->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("l3")==0)
			l3Box->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("rd")==0)
			RDBox->setCheckState(Qt::Checked);
		else if(ProfileParam[i].compare("ga")==0)
			GABox->setCheckState(Qt::Checked);
	}
}

bool DTIAtlasFiberAnalyzerguiwindow::GetAutoPlaneOption()
{
	if(APMedianOnFiber->isChecked())
		return false;
	else
		return true;
}

/***************************************************
 * 					TAB 4
 ***************************************************/

 /************************************************************************************
 * setParamFromFile : Read file and find out used parameters to finally put them
 * 	in m_parameters string and adjust m_NumberOfParameters.
 ************************************************************************************/

void DTIAtlasFiberAnalyzerguiwindow::setParamFromFile(std::string filepath)
{
	vstring param;
	m_parameters=getParamFromFile(filepath);
	LineInVector(m_parameters, param);
	m_NumberOfParameters=param.size();
}

	

 /************************************************************************************
 * setFibers : Fill the m_Fibers string vector with selected fiber's names
 ************************************************************************************/

void DTIAtlasFiberAnalyzerguiwindow::setFibers()
{
	std::string fiber;
	m_Fibers.clear();
	for(unsigned int j=0; j<m_CSV->getColSize(0); j++)
	{
		if(ExtensionofFile((*m_CSV->getData())[0][j]).compare("fvp")==0)
		{
			fiber=takeoffExtension((*m_CSV->getData())[0][j]);
			m_Fibers.push_back(fiber);
		}
	}
	sort(m_Fibers.begin(), m_Fibers.end());
}

 /************************************************************************************
 * setCases : Fill the m_Cases string vector with case's names from .csv
 ************************************************************************************/

void DTIAtlasFiberAnalyzerguiwindow::setCases()
{
	std::string casename;
	m_Cases.clear();
	for(unsigned int row=1; row<m_CSV->getRowSize(); row++)
	{
		casename=NameOfCase(m_CSV, row, m_NameCol, m_DataCol);
		m_Cases.push_back(casename);
	}
}
	
 
 /************************************************************************************
 * ReadDataFilesNameInCaseDirectory : Fill the datafiles vector with every file name
 * 	in the Case directory for relevant fibers.
 ************************************************************************************/
 
void DTIAtlasFiberAnalyzerguiwindow::ReadDataFilesNameInDirectory(vstring &datafiles, std::string Dir)
{
	std::string nameofcase=Dir.substr(Dir.find_last_of("/")+1, Dir.size()-Dir.find_last_of("/")+1);
	itksys::Directory directory;
	directory.Load(Dir.c_str());
	unsigned long NumberOfFiles = directory.GetNumberOfFilesInDirectory(Dir.c_str());
	std::string filename, extensionoffile, fiber, filefiber;
	
	//save the name of the Data Files in an vstring
	for(int j=0; j<FiberSelectedList->count(); j++)
	{
		fiber=takeoffExtension(((FiberSelectedList->item(j))->text()).toStdString());
		for( unsigned long i=0;i<NumberOfFiles;i++)
		{
			filename=directory.GetFile(i);
			filefiber=takeoffExtension(filename);
			if(filefiber.size()!=0)
			{
				filefiber=filefiber.substr(nameofcase.size()+1, filefiber.size()-nameofcase.size()+1);
				extensionoffile = ExtensionofFile(filename);
				if(extensionoffile.compare("fvp")==0 && filefiber.compare(0, filefiber.size(), fiber)==0)
					datafiles.push_back(filename);
			}
		}
	}
}
 
 /************************************************************************************
 * FillDataFilesList : Fill the DataFiles QWidgetList with relevent .fvp files
 ************************************************************************************/
 
void DTIAtlasFiberAnalyzerguiwindow::FillDataFilesList()
{
	vstring datafiles;
	std::string casename, Dir, Item;
	setFibers();
	
	for(unsigned int row=1; row<m_CSV->getRowSize(); row++)
	{
		casename=NameOfCase(m_CSV, row, m_NameCol, m_DataCol);
		Dir=m_OutputFolder + "/Cases/" + casename;
		ReadDataFilesNameInDirectory(datafiles, Dir);
	}
	
	//Print the name in the ListWidget
	for(unsigned int j=0;j<datafiles.size();j++)
	{
		//Avoid double definitions
		if(DataFilesList->findItems(datafiles[j].c_str(), Qt::MatchExactly).size()==0)
			DataFilesList->addItem(datafiles[j].c_str());
	}
}

/************************************************************************************
 * OpenPlotWindow : Get and calculate every data samples and call PlotWindow class
 * 	Data should be stored in a 4D vector as following :
 * 		1st dimension correspond to each fiber.
 * 		2nd dimension correspond to each data table. For case data, each case have 
 * 			3 data table in this order: Value, Value+Std, Value-Std. For Atlas data
 * 			there are 4 tables : Value, Value+Std, Value-Std, Nb of fiber points.
 * 			For Stat data there are 3 tables: Cross Mean, Cross Mean+Cross Std, 
 * 			Cross Mean-Cross Std.
 * 		3rd and 4th dimensions correspond to lines and columns of data samples.
 ************************************************************************************/

bool DTIAtlasFiberAnalyzerguiwindow::OpenPlotWindow()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	std::cout<<"Begining of PlotWindow function..."<<std::endl;
	
	
	qv3double fiberdata;
	QVector <QVector <double> > data, temp;
	std::string filepath, casename, filename; 
	
	m_PlotError=false;
	
	//Clear all relevant variables
	m_parameters.clear();
	m_Cases.clear();
	m_Fibers.clear();
	m_casedata.clear();
	m_atlasdata.clear();
	m_statdata.clear();	
	
	//Fill m_parameters, m_Cases, and m_Fibers vector
	setCases();
	setFibers();
	filepath=m_OutputFolder + "/Fibers/" + m_Fibers[0] + ".fvp";
	setParamFromFile(filepath);	//Filled from the atlas file
	
	if(m_parameters.size()==0)
	{
		std::cout<<"parameters vector error"<<std::endl;
		QApplication::restoreOverrideCursor();
		return false;
	}	
	
	for(unsigned int i=0; i<m_Fibers.size(); i++)
	{
		for(unsigned int j=0; j<m_Cases.size(); j++)
		{
			filename=m_Cases[j]+"_"+m_Fibers[i]+".fvp";
			filepath=m_OutputFolder+"/Cases/"+m_Cases[j]+"/"+filename;
			data=TableConversion(m_FiberProfile[i][j]);
			if(m_PlotError)
			{
				std::cout<<"Plot function canceled."<<std::endl;
				QApplication::restoreOverrideCursor();
				return false;
			}
			
			fiberdata.push_back(data);
			
			//Filling m_casedata with data samples + std
			temp=getStdData(data, 1);
			fiberdata.push_back(temp);
			//Filling m_casedata with data samples - std
			temp=getStdData(data, -1);
			fiberdata.push_back(temp);
		}
		m_casedata.push_back(fiberdata);
		fiberdata.clear();
		
		filename=m_Fibers[i];
		filepath=m_OutputFolder+"/Fibers/" + filename + ".fvp";
		data=TableConversion(m_FiberProfile[i][m_FiberProfile[i].size()-2]);
		
		if(m_PlotError)
		{
			std::cout<<"Plot function canceled."<<std::endl;
			QApplication::restoreOverrideCursor();
			return false;
		}
		
		fiberdata.push_back(data);
		
		//Filling m_atlasdata with data samples + std
		temp=getStdData(data, 1);
		fiberdata.push_back(temp);
		
		//Filling m_atlasdata with data samples - std
		temp=getStdData(data, -1);
		fiberdata.push_back(temp);
		
		//Filling m_atlasdata with nb of fiber points
		data=TableConversion(m_FiberProfile[i][m_FiberProfile[i].size()-1]);
		fiberdata.push_back(data);
		
		m_atlasdata.push_back(fiberdata);
		fiberdata.clear();
		
		//Filling m_statdata with mean samples
		data=getCrossMeanData(m_casedata[i]);
		fiberdata.push_back(data);
		
		//Filling m_statdata with mean samples + std
		temp=getCrossStdData(m_casedata[i], data, 1);
		fiberdata.push_back(temp);
		
		//Filling m_statdata with mean samples - std
		temp=getCrossStdData(m_casedata[i], data, -1);
		fiberdata.push_back(temp);
		
		m_statdata.push_back(fiberdata);
		fiberdata.clear();
	}
	
	//Open Plot Window
	std::cout<<"Opening Plot Window..."<<std::endl;
	m_plotwindow=new PlotWindow(m_casedata, m_atlasdata, m_statdata, m_parameters, this);
	m_plotwindow->setWindowTitle("Selected curves' plot");
	m_plotwindow->setAttribute(Qt::WA_DeleteOnClose, true);
	if(m_plotwindow->getError())
	{
		QApplication::restoreOverrideCursor();
		return false;
	}
	
	QApplication::restoreOverrideCursor();
	return true;
}

/************************************************************************************
* TableConversion : Converts table from std::string to double
 ************************************************************************************/

QVector< QVector<double> > DTIAtlasFiberAnalyzerguiwindow::TableConversion(v2string table)
{
	QVector <QVector <double> > data;
	QVector <double> line;
	for(unsigned int i=0; i<table.size(); i++)
	{
		for(unsigned int j=0; j<table[i].size(); j++)
			line.push_back(atof(table[i][j].c_str()));
		data.push_back(line);
		line.clear();
	}
	return data;
}

/************************************************************************************
* getFiberInformations : Gets 6 first lines in a .fvp file 
 ************************************************************************************/
 
vstring DTIAtlasFiberAnalyzerguiwindow::getFiberInformations(std::string fiber)
{
	vstring parameterslines;
	std::string buffer, filepath;

//Open first .fvp file corresponding to string fiber (the case doesn't matter)
	filepath=m_OutputFolder+"/Cases/"+m_Cases[0]+"/"+m_Cases[0]+"_"+fiber+".fvp";
	std::ifstream fvpfile(filepath.c_str(), std::ios::in);

//Put first line in buffer
	getline(fvpfile, buffer);

//Stop at last line of information or if it's the end of file
	while(buffer.compare(0,35,"Number of samples along the bundle:")!=0 && !fvpfile.eof())
	{
		parameterslines.push_back(buffer);
		getline(fvpfile,buffer);
	}
	parameterslines.push_back(buffer);
	return parameterslines;
}


/***************************************************
 * 				MergeStatWithFiber Tab
 ***************************************************/
 
 
void DTIAtlasFiberAnalyzerguiwindow::BrowserDTIPCsvFilename()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QString type;
	QString filename=QFileDialog::getOpenFileName(this, "Open CSV File", "/", "Text (*.csv)", &type);
	DTIPcsvfilename->setText(filename);
	CheckNextStep();
	
	QApplication::restoreOverrideCursor();	
}

void DTIAtlasFiberAnalyzerguiwindow::BrowserDTIPVtkFilename()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QString type;
	QString filename=QFileDialog::getOpenFileName(this, "Open VTK File", "/", "VTK (*.vtk)", &type);
	DTIPvtkfilename->setText(filename);
	CheckNextStep();
	
	QApplication::restoreOverrideCursor();
}

void DTIAtlasFiberAnalyzerguiwindow::BrowserDTIPOutputFilename()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	QString type;
	QString filename=QFileDialog::getSaveFileName(this, "Save File", ".", "VTK (*.vtk)");
	DTIPoutputfilename->setText(filename);
	CheckNextStep();
	
	QApplication::restoreOverrideCursor();
}

bool DTIAtlasFiberAnalyzerguiwindow::ComputeDTIParametrization()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	std::string PathMergeStatWithFiber = itksys::SystemTools::FindProgram("MergeStatWithFiber");
	std::cout<<PathMergeStatWithFiber<<std::endl;
	if(PathMergeStatWithFiber.size()==0)
	{
		QMessageBox::warning(this, "MergeStatWithFiber", "Select the folder where MergeStatWithFiber is located.");
		PathMergeStatWithFiber=QFileDialog::getExistingDirectory(this).toStdString();
		PathMergeStatWithFiber=PathMergeStatWithFiber+"/MergeStatWithFiber";
	}
	if(PathMergeStatWithFiber.size()!=0)
	{
		if(CallMergeStatWithFiber(PathMergeStatWithFiber, DTIPcsvfilename->text().toStdString(), DTIPvtkfilename->text().toStdString(), DTIPoutputfilename->text().toStdString(), DTIP_LE_Min->text().toStdString(), DTIP_LE_Max->text().toStdString())!=0)
		{
			std::cout<<"Fail during MergeStatWithFiber!"<<std::endl;
			return false;
		}
	}
	
	QApplication::restoreOverrideCursor();
	return true;
}



/***************************************************
 * 					File Menu Slots
 ***************************************************/

/********************************************************************************* 
 * Save the value of the parameters for data or analysis
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::SaveDataAction()
{
	QString output;
	if(m_OutputFolder.size()!=0)
		output=m_OutputFolder.c_str();
	else
		output=".";
	QString filename = QFileDialog::getSaveFileName(this, "Save data file", output, "Text (*.txt)");
	SaveData(filename.toStdString(), m_csvfilename, m_DataCol, m_DeformationCol, m_NameCol, m_OutputFolder);
}

void DTIAtlasFiberAnalyzerguiwindow::SaveAnalysisAction()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save parameters file", QString(), "Text (*.txt)");
	SaveAnalysis(filename.toStdString(), m_AtlasFiberDir, m_FiberSelectedname, m_parameters, m_transposeColRow);
}

/********************************************************************************* 
 * Load a data file.
 ********************************************************************************/
void DTIAtlasFiberAnalyzerguiwindow::OpenDataFile()
{
	bool loaded = false, DataColDone=false, NameColDone=false, DefColDone=false;
	QString filename = QFileDialog::getOpenFileName(this, "Open Data File", QString(), "Text (*.txt)");
	
	std::string str;
	//clear the previous data values
	ClearDataInformation();
	
	if(filename!=NULL)
	{
		std::ifstream file((filename.toStdString()).c_str() , std::ios::in);  // open in reading
		std::string str,buf1,NameCol, DefCol, DataCol;
		
		if(file)  // if open
		{
			//the first line
			getline(file, buf1);
			if(buf1.compare(0,44,"Data parameters for DTIAtlasFiberAnalyzer : ")==0)
			{
				/* Enable the frame to add column and row */
				addCRframe->setEnabled (true);
				saveframe->setEnabled (true);
				ColumnFrame->setEnabled (true);
				/* Loop for reading the file and setting the parameters values */
				while(!file.eof())
				{
					getline(file, buf1);
					//Skip commentaries
					if(buf1.compare(0,1,"#")!=0)
					{
						if(buf1.compare(0,10,"CSVFile : ")==0)
						{
							if(buf1.compare(10,5,"")!=0)
							{
								m_csvfilename = buf1.substr(10,buf1.size()-10);
								csvfilename->setText(m_csvfilename.c_str());
								if(!m_CSVcreated)
								{
									m_CSV = new CSVClass(m_csvfilename,m_debug);
									m_CSVcreated = true;
								}
								if(m_CSV->loadCSVFile(m_csvfilename))
								{
									FillCSVFileOnQTable();
									loaded = true;
								}
							}
						}
						else if(buf1.compare(0,14,"Data Column : ")==0)
						{
							if(buf1.compare(14,5,"")!=0)
							{
								DataCol = buf1.substr(14,buf1.size()-14);
								m_DataCol = atoi(DataCol.c_str())-1;
								DataColDone=true;
							}
						}
						else if(buf1.compare(0,27,"Deformation Field Column : ")==0)
						{
							if(buf1.compare(26,5,"")!=0)
							{
								DefCol = buf1.substr(26,buf1.size()-26);
								m_DeformationCol = atoi(DefCol.c_str())-1;
								DefColDone=true;
							}
						}
						else if(buf1.compare(0,14,"Case Column : ")==0)
						{
							if(buf1.compare(14,5,"")!=0)
							{
								NameCol = buf1.substr(14,buf1.size()-14);
								m_NameCol = atoi(NameCol.c_str())-1;
								NameColDone=true;
							}
						}
						else if(buf1.compare(0,16,"Output Folder : ")==0 && buf1.compare(16,5,"")!=0)
						{
							m_OutputFolder = buf1.substr(16,buf1.size()-16);
							outputfolder->setText(m_OutputFolder.c_str());
						}
						else if(buf1.size()!=0)
						{
							QMessageBox::warning(this, "Warning", "Syntax error in chosen data file!");
							std::cout<<"Error at this line : "<<buf1<<std::endl;
							ClearDataInformation();
							return;
						}
					}
				}
				
				//CSV needs to be loaded before selecting specific columns
				if(loaded)
				{
					if(DataColDone)
					{
						ColumnData->setText(DataCol.c_str());
						ColorCol(1);
					}
					if(DefColDone)
					{
						ColumnDeformation->setText(DefCol.c_str());
						ColorCol(2);
					}
					if(NameColDone)
					{
						ColumnNameCases->setText(NameCol.c_str());
						ColorCol(3);
					}
				}
				else if(!loaded && (DataColDone || DefColDone || NameColDone))
					QMessageBox::warning(this, "Warning", "CSVFilename required to choose specific column.");
				
				//next step true if there is an csv and an output
				if(loaded && csvfilename->text().compare("")!=0 && outputfolder->text().compare("")!=0 && m_CSVcreated)
					m_nextStep = true;
				
				std::cout<<std::endl;
				std::cout<<"Data file loaded : New Data"<<std::endl;
				CheckNextStep();
			}
			else
				std::cout<<"ERROR: Wrong file for data parameters"<<std::endl;
			file.close();
		}
		else std::cerr << "ERROR: No data parameters file found" << std::endl;
	}
	
}

/********************************************************************************* 
 * Load an analysis file.
 ********************************************************************************/

void DTIAtlasFiberAnalyzerguiwindow::OpenAnalysisFile()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open Analysis File", QString(), "Text (*.txt)");
	LoadAnalysisFile(filename.toStdString());
}

void DTIAtlasFiberAnalyzerguiwindow::LoadAnalysisFile(std::string filename)
{
	bool AtlasDone=false, SelectedFiberDone=false, SelectedPlaneDone=false;
	std::string ListOfFiber, str, ListOfPlane;
	std::vector<int> plane;
	
	if(filename.size()!=0)
	{
		std::ifstream file(filename.c_str() , std::ios::in);  // open in reading
		std::string str,buf1;
		
		if(file)  // if open
		{
				//the first line
			getline(file, buf1);
			if(buf1.compare(0,48,"Analysis parameters for DTIAtlasFiberAnalyzer : ")==0)
			{
				/* Loop for reading the file and setting the parameters values */
				while(!file.eof())
				{
					getline(file, buf1);
					//Skip commentaries
					if(buf1.compare(0,1,"#")!=0)
					{
						if(buf1.compare(0,21,"Atlas Fiber Folder : ")==0)
						{
							if(buf1.compare(21,5,"")!=0)
							{
								FiberInAtlasList->clear();
								m_AtlasFiberDir = buf1.substr(21,buf1.size()-21);
								AtlasFiberFolder->setText(m_AtlasFiberDir.c_str());
								FillFiberFrame();
								AtlasDone=true;
							}
						}
						else if(buf1.compare(0,18,"Selected Fibers : ")==0)
						{
							if (buf1.compare(18,5,"")!=0)
							{
								FiberSelectedList->clear();
								FiberSelected2List->clear();
								FiberPlaneFile->clear();
								m_FiberSelectedname.clear();
								m_RelevantPlane.clear();
								ListOfFiber = buf1.substr(18,buf1.size()-18);
										// take the name of each fiber
								LineInVector(ListOfFiber, m_FiberSelectedname);
								SelectedFiberDone=true;
							}
						}
						else if(buf1.compare(0,18,"Selected Planes : ")==0)
						{
							if (buf1.compare(18,5,"")!=0)
							{
								m_SelectedPlane.clear();
								ListOfPlane=buf1.substr(18, buf1.size()-18);
								LineInVector(ListOfPlane, m_SelectedPlane);
								SelectedPlaneDone=true;
							}
						}
						else if(buf1.compare(0,20,"Profile parameter : ")==0)
						{
							m_parameters = buf1.substr(20,buf1.size()-20);
							m_NumberOfParameters = CalculNumberOfProfileParam(m_parameters);
							if(m_parameters.compare("")!=0)
								checkBoxProfile(m_parameters);
						}
						else if(buf1.compare(0,14,"Col and Row : ")==0)
						{
							if(buf1.substr(14,buf1.size()-14).compare("Case in column")==0 ||buf1.substr(14,buf1.size()-14).compare("Case in col")==0 ||buf1.substr(14,buf1.size()-14).compare("Arc lenght in row")==0)
							{
								m_transposeColRow = true;
								CaseOnArc->setChecked(true);
							}
							else
							{
								m_transposeColRow = false;
								ArcOnCase->setChecked(true);
							}
						}
						else if(buf1.compare(0,20,"Auto Plane Origin : ")==0)
						{
							if(buf1.compare(20, 6, "median")==0)
								APMedianOnFiber->setChecked(true);
							else if(buf1.compare(20,3,"cog")==0 || buf1.compare(20,5,"")==0)
								APCenterOfGravity->setChecked(true);
						}
						else if(buf1.size()!=0)
						{
							QMessageBox::warning(this, "Warning", "Syntax error in chosen analysis file!");
							std::cout<<"Error at this line : "<<buf1<<std::endl;
							FiberInAtlasList->clear();
							FiberSelectedList->clear();
							FiberSelected2List->clear();
							FiberPlaneFile->clear();
							m_Fibername.clear();
							m_FiberSelectedname.clear();
							m_Fibersplane.clear();
							m_RelevantPlane.clear();
							return;
						}
					}
				}
				//Before selecting fibers an Atlas folder should be specified
				if(AtlasDone)
				{
					if(SelectedFiberDone)
					{
						bool found;
						for(unsigned int i=0; i<m_FiberSelectedname.size(); i++)
						{
							for(unsigned int j=0; j<m_Fibername.size(); j++)
							{
								if(m_FiberSelectedname[i]==m_Fibername[j])
									found=true;
							}
							if(!found)
							{
								m_FiberSelectedname.erase(m_FiberSelectedname.begin()+i);
								i--;
							}
						}							
						for(unsigned int i=0;i<m_FiberSelectedname.size();i++)
						{
							//remove the name in the first vector and the list
							for(unsigned int k=0;k<m_Fibername.size();k++)
							{
								if(m_Fibername[k].compare(m_FiberSelectedname[i])==0)
								{
									FiberSelectedList->addItem(m_FiberSelectedname[i].c_str());
									FiberSelected2List->addItem(m_FiberSelectedname[i].c_str());
									FiberInAtlasList->takeItem(k);
								}
							}
							/* Check the plane and add/erase it */
							plane = PlaneAssociatedToFiber(m_FiberSelectedname[i], m_Fibersplane);
							if(plane.size()!=0)
							{
								for(unsigned int k=0; k<plane.size(); k++)
								{
									m_RelevantPlane.push_back(m_Fibersplane[plane[k]]);
									QListWidgetItem* itemplane= new QListWidgetItem;
									QString qs(m_Fibersplane[plane[k]].c_str());
									itemplane->setData( 0, qs );
									FiberPlaneFile->addItem(itemplane);
									m_Fibersplane.erase(m_Fibersplane.begin()+plane[k]);
								}
							}
						}
						
						//Select specified planes or all of them if not.
						if(!SelectedPlaneDone)
							m_SelectedPlane=m_RelevantPlane;
						
						for(unsigned int i=0; i<m_RelevantPlane.size(); i++)
						{
							for(unsigned int j=0; j<m_SelectedPlane.size(); j++)
							{
								if(m_RelevantPlane[i]==m_SelectedPlane[j])
									FiberPlaneFile->item(i)->setSelected(true);
							}
						}
							
					}
					
				}
				else if(!AtlasDone && SelectedFiberDone)
					QMessageBox::warning(this, "Warning", "Atlas directory should be specified if fibers are selected.");
				else if(!SelectedFiberDone && SelectedPlaneDone)
					QMessageBox::warning(this, "Warning", "Selected Fibers should be specified if planes are selected.");
					
				std::cout<<std::endl;
				std::cout<<"Analysis file loaded."<<std::endl;
				CheckNextStep();
			}
			else
				std::cout<<"ERROR: Wrong file for analysis parameters"<<std::endl;
			file.close();
		}
		else std::cerr << "ERROR: No analysis parameters file found" << std::endl;
	}
}

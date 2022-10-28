#include <QCoreApplication>

#include <QDir>
#include <QProcess>
#include <QRegularExpression>

const char P4IGNORE[] = ".p4ignore";

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QDir d( a.applicationDirPath() );

	// Set the default P4IGNORE value
	{
		qDebug()<<"Setting default ignore file...";
		system( ( QString("p4 set P4IGNORE=") + P4IGNORE ).toStdString().c_str() );
	}

	// We need to move into the root repo frmo our tools directory
	{
		qDebug()<<"Finding Project Root...";
		for(;;)
		{
			d.cdUp();
			if( QFile::exists( d.absoluteFilePath( P4IGNORE ) ) )
			{
				qDebug()<<"Found workspace: " << d.dirName();
				// Necessary to change the system default client, because -i does not take a -t parameter, but -o technically does
				system( QString("p4 set P4CLIENT=" + d.dirName()).toStdString().c_str() );
				break;
			}
		}
	}

	// Fetch the current config for the workspace
	//{
		auto GetConfig = []() {
			QProcess p;
			p.start("p4", { "client", "-o" } );
			p.waitForFinished();

			return QString::fromUtf8( p.readAllStandardOutput() );
		};

		qDebug()<<"Fetching data...";
		const auto& StdOutput = GetConfig();

		auto Entries { StdOutput.split("\n", Qt::SkipEmptyParts) };
	//}

	// Go over the config and change entries to the desired values
	{
		qDebug()<<"Modifying Data...";
		for( auto& Entry : Entries )
		{
			// Prune [Carriage Return] if present
			if( Entry.length() && Entry.back() == '\r')
			{
				Entry.chop(1);
			}

			// Revert unchanged files so we don't get unnecessary erroneous changelist contents
			if( Entry.startsWith("SubmitOptions:") )
			{
				Entry = "SubmitOptions: revertunchanged";
				continue;
			}

			if( Entry.startsWith("Options:") )
			{
				auto Options { Entry.split( QRegularExpression("\\s+"), Qt::SkipEmptyParts) };
				for( auto& Option : Options )
				{
					// Compress files to optimize for network traffic
					if( Option == "nocompress" )
					{
						Option = "compress";
						continue;
					}

					// Automatically prune empty directories when deleting files
					if( Option == "normdir" )
					{
						Option = "rmdir";
						continue;
					}
				}

				// Recombine changes into entry
				Entry = Options.join(' ');
			}
		}
	}


	// Save Changes
	{
		auto SetConfig = []( const QString& StdInput ) {
			QProcess p;
			p.start("p4", { "client", "-i" } );
			p.waitForStarted(15000);

			p.write(StdInput.toUtf8());
			p.waitForBytesWritten();
			p.closeWriteChannel();

			p.waitForReadyRead(15000);

			qDebug()<<p.readAllStandardOutput();
		};

		qDebug()<<"Saving data...";
		SetConfig( Entries.join("\n") );
	}

	qDebug()<<"Success! You can close the window now.";
	return a.exec();
}

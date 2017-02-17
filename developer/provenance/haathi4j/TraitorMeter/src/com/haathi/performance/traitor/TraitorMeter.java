/* ***************************************************************************
 $Header: //MyDataDepot/Projects/provenance-autotype2/haathi4j/TraitorMeter/src/com/haathi/performance/traitor/TraitorMeter.java#3 $
 *****************************************************************************
 Package
 ****************************************************************************/
package com.haathi.performance.traitor;
/* ***************************************************************************
 Imports
 ****************************************************************************/
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintStream;
import java.nio.file.Paths;
import java.util.ArrayList;

import com.haathi.performance.SharedMeter;
import com.haathi.process.ExternalProcess;
import com.haathi.watermark.analyzer.TraitorAnalyzer;
/* ***************************************************************************
 Class
 ****************************************************************************/
public class TraitorMeter extends SharedMeter {	
	public static final String GHOST_SCRIPT_COMMAND = "D:\\ProgramFiles\\gs\\gs9.18\\bin\\gswin64.exe";
	public static final String TESSERACT_DIR = "..\\..\\libcpp\\tesseract";
	public static final String DICTIONARY = "dictionary.txt";
	public static final int RANDOM_COUNT = 50000;
	
	protected boolean validSyntax;
	
	public TraitorMeter(String[] args) {
		super(args);
		validSyntax = validSyntax();

		if (!validSyntax)
			printUsage();
	}

	protected boolean isValid() {
		return validSyntax;
	}
	
	protected boolean validSyntax() {
		return true;
	}
	
	protected void printUsage() {
		System.out.print("Usage: " + this.getClass().getName());
	}

	protected void clear() {
	}
	
	public void runPdfToJpg(int page, int r, int q, String docPdf, String docJpg) throws Exception {
		ArrayList<String> command = new ArrayList<String>();
		command.add(GHOST_SCRIPT_COMMAND);
		command.add("-dSAFER");
		command.add("-dBATCH");
		command.add("-dNOPAUSE");
		command.add("-dTextAlphaBits=4");
		command.add("-dGraphicsAlphaBits=4");
		if (page != 0) {
			command.add("-dFirstPage=" + page);
			command.add("-dLastPage=" + page);
		}
		command.add("-r" + Integer.toString(r));
		command.add("-dJPEGQ=" + Integer.toString(q));
		command.add("-sDEVICE=jpeg");
		command.add("-sOutputFile=" + resDir + docJpg);
		command.add(resDir + docPdf);

		ExternalProcess process = new ExternalProcess(command);
		process.execute();	
	}
	
	protected void runWatermarkBitExtractor(String docJpg, String inWm, String outWm, String outTxt) throws Exception {
		String watermarkBitExtractorCmd = "..\\..\\haathi\\watermarkBitExtractor\\Debug-DynLink\\watermarkBitExtractor.exe";
		String tesseractDir = Paths.get(curDir + File.separator + TESSERACT_DIR).toAbsolutePath().normalize().toString();
		
		watermarkBitExtractorCmd = Paths.get(watermarkBitExtractorCmd).toAbsolutePath().normalize().toString();
		ArrayList<String> command = new ArrayList<String>();
		command.add(watermarkBitExtractorCmd);
		command.add(resDir + docJpg);
		command.add(resDir + inWm);
		command.add(resDir + outWm);
		command.add(resDir + outTxt);
		command.add(resDir + DICTIONARY);
	
		ExternalProcess process = new ExternalProcess(command);
		process.directory(tesseractDir);		
		process.execute();	
	}

	protected String fromLastPlus(String text) {
		int index = text.lastIndexOf('+');
		
		return index < 0 ? text : text.substring(index);
	}
	
	protected String resultName(int users, int documents) {
		return "\"" + users + "u" + documents + "d\"";
	}

	protected void printResults(PrintStream printStream, int s, String label, int[] users, int[] documents, long[] times) {
		if (printStream != System.out)
			printResults(System.out, s, label, users, documents, times);
		printStream.println();
		printStream.println("#Name\tUsers\tDocuments\tTheories\tTime");
		for (int i = 0; i < 5; i++)
			printStream.print(label);
		printStream.println();

		for (int i = 0; i < users.length; i++) {
			printStream.print(resultName(users[i], documents[i]));
			printStream.print("\t");
			printStream.print(users[i]);
			printStream.print("\t");
			printStream.print(documents[i]);
			printStream.print("\t");
			printStream.print(users[i] * documents[i]);
			printStream.print("\t");
			printStream.print((double) times[i] / 1000);
			printStream.print("\n");
		}		
		printStream.println();
	}

	protected static String incString(String text, int inc) {
		int pos = inc % text.length();
		int val = inc / text.length();
		char c = (char) (text.charAt(pos) + (char) val);
		
		return text.substring(0, pos) + c + text.substring(pos + 1);		
	}
	
	public static class User {
		public int userId;
		public String privateKey;
		
		public User(ArrayList<String> headers, String line) {
			String[] values = line.split("\t");
			
			userId = Integer.parseInt(values[headers.indexOf("UserId")]);
			privateKey = values[headers.indexOf("PrivateKey")];
		}
		
		public String getUserId(int inc) {
			return Integer.toString(userId + inc);
		}
		
		public String getPrivateKey(int inc) {
			if (inc == 0)
				return privateKey;
			
			return incString(privateKey, inc);
		}
	}

	public static class Document {
		public int documentId;
		public int versionNo;
		public String versionId;

		public Document(ArrayList<String> headers, String line) {
			String[] values = line.split("\t");
			
			documentId = Integer.parseInt(values[headers.indexOf("DocumentId")]);
			versionNo = Integer.parseInt(values[headers.indexOf("VersionNo")]);			
			versionId = values[headers.indexOf("VersionId")];			
		}
		
		public String getDocumentId(int inc) {
			return Integer.toString(documentId + inc);
		}
		
		public String getVersionNo(int inc) {
			return Integer.toString(versionNo + inc);
		}
		
		public String getVersionId(int inc) {
			if (inc == 0)
				return versionId;
			return incString(versionId, inc);
		}
	}
	
	protected void printUserDocument(User user, int userInc, Document document, int documentInc, BufferedWriter bufferedWriter) throws IOException {
		bufferedWriter.write(user.getUserId(userInc));
		bufferedWriter.write("\t");
		bufferedWriter.write(document.getDocumentId(documentInc));
		bufferedWriter.write("\t");
		bufferedWriter.write(document.getVersionNo(documentInc));
		bufferedWriter.write("\t");
		bufferedWriter.write(document.getVersionId(documentInc));
		bufferedWriter.write("\t");
		bufferedWriter.write(user.getPrivateKey(userInc));
		bufferedWriter.write("\n");
	}
	
	protected long runTheories(int i, int users, int documents, String outWm, String outWmc, String inTh, String inThs, String outEvs) throws IOException, InterruptedException {
		String tmpInTh = String.format(inThs, i + 1);
		String tmpOutEv = String.format(outEvs, i + 1);
		BufferedReader inThBufferedReader = null;
		BufferedWriter tmpInThBufferedWriter = null;
		
		try {
			inThBufferedReader = new BufferedReader(new FileReader(resDir + inTh));
			tmpInThBufferedWriter = new BufferedWriter(new FileWriter(resDir + tmpInTh));
		
			ArrayList<String> headers = getHeaders(inThBufferedReader);
			putHeaders(headers, tmpInThBufferedWriter);
			
			String line = inThBufferedReader.readLine();

			User user = new User(headers, line);
			Document document = new Document(headers, line);
			
			for (int userInc = 0; userInc < users; userInc++)
				for (int documentInc = 0; documentInc < documents; documentInc++)
					printUserDocument(user, userInc, document, documentInc, tmpInThBufferedWriter);		
		}
		finally {
			if (inThBufferedReader != null)
				inThBufferedReader.close();
			if (tmpInThBufferedWriter != null)
				tmpInThBufferedWriter.close();
		}
		
		TraitorAnalyzer traitorAnalyzer = new TraitorAnalyzer(RANDOM_COUNT, resDir + outWm, resDir + outWmc, resDir + tmpInTh, resDir + tmpOutEv, true);
		long elapsedTime = traitorAnalyzer.runTraitorTracer();
		
		return elapsedTime;
	}

	protected void printAtATime(PrintStream printStream) {
		int[] users = new int[]     { 1, 2, 1, 2, 8, 8, 8, 16, 16 }; //, 32 };
		int[] documents = new int[] { 1, 1, 4, 4, 2, 4, 8,  8, 16 }; // , 16 };
		long[] times = new long[users.length];
		
		String docTxt = "DofI.txt"; //  "Twain.txt" /*,  "Turing.txt" , "SofU.txt"*/ };
		String inTh = "in.th";
		String inThs = "in%d.th";

		String docPdf = "doc.pdf";		
		String docOdt = "doc.odt";
		String docJpg = "doc.jpg";
		String inWm = "in.wm";
		String inTxt = "in.txt";
		String outWm = "out.wm";
		String outTxt = "out.txt";
		String outWmc = "out.wmc";
		String outEvs = "out%d.ev";

		int r = 400;
		int q = 90;
		int v = 2;
		int p = 1; // just do first page
		
		int[] ses = { 63, 1, 2, 4, 8, 16, 32 }; // Do total then individuals
		int[] indexes = {  6, 0, 1, 2, 3,  4,  5 }; // Map to orderedKeys
		
		try {
			for (int i = 0; i < ses.length; i++) {
				int s = ses[i];

				clear();
				eraseResFiles(new String[] { docPdf, docOdt, docJpg, inWm, inTxt, outWm, outWmc, outTxt });
				eraseMultiResFiles(new String[] { inThs, outEvs });
				runAutoPdf(s, v, docTxt, docPdf, docOdt, inWm, inTxt);
				runPdfToJpg(p, r, q, docPdf, docJpg);
				runWatermarkBitExtractor(docJpg, inWm, outWm, outTxt);
				
				TraitorAnalyzer traitorAnalyzer = new TraitorAnalyzer(RANDOM_COUNT, resDir + outWm, resDir + outWmc, resDir + inTh, resDir + outEvs, false);
				traitorAnalyzer.prepareWmcOut(p); // just page 1
				
				for (int j = 0; j < users.length; j++)
					times[j] = runTheories(j, users[j], documents[j], outWm, outWmc, inTh, inThs, outEvs);
		
				eraseResFiles(archive(docTxt, s), new String[] { docPdf, docOdt, docJpg, inWm, inTxt, outWm, outWmc, outTxt });
				eraseMultiResFiles(archive(docTxt, s), new String[] { inThs, outEvs });
				printResults(printStream, s, orderedKeys[indexes[i]], users, documents, times);
			}
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
		}
	}

	protected void plotAtATime() {
		String ttInPlt = "tt.dat";
		
		String traitorPlt = "traitor.plt";
		String traitorOutPlt = "traitor.png";
		
		try {
			eraseResFiles(new String[] { ttInPlt, traitorOutPlt });

			PrintStream printStream = new PrintStream(new File(resDir + ttInPlt));
			printAtATime(printStream);
			printStream.close();			
			
			plotResults(traitorPlt, ttInPlt, traitorOutPlt);

			eraseResFiles(archive(""), new String[] { ttInPlt, traitorOutPlt });			
		}
		catch (Exception exception) {
			exception.printStackTrace(System.err);
			System.exit(-1);			
		}
	}	
	/* ***************************************************************************
	 Main
	 ****************************************************************************/		
	public void run() {
		if (!isValid())
			return;
		
		//printAtATime(System.out);
		plotAtATime();
	}
	
	public static void main(String[] args) {
		new TraitorMeter(args).run();
	}	
}
/* **************************************************************************/

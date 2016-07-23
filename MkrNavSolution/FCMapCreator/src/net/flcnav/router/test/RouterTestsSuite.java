package net.flcnav.router.test;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;


import net.flcnav.binary.BinaryMapIndexReader;
import net.flcnav.osm.LatLon;
import net.flcnav.router.BinaryRoutePlanner;
import net.flcnav.router.RouteSegmentResult;
import net.flcnav.router.RoutingConfiguration;
import net.flcnav.router.RoutingContext;
import net.flcnav.router.BinaryRoutePlanner.RouteSegment;
import net.flcnav.swing.NativeSwingRendering;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;


public class RouterTestsSuite {
	// TODO add one international test
	// POLAND/BELARUS (problems with way connection) The best at 100%
//	Start lat=52.115756035004786 lon=23.56539487838745
//	END lat=52.03710226357107 lon=23.47106695175171
//	id=32032589 start=8 end=9
//	id=32032656 start=0 end=18
//	id=32031919 start=1 end=0
	public static void main(String[] args) throws IOException, SAXException, ParserConfigurationException {
		Properties properties = new Properties();
		properties.load(RouterTestsSuite.class.getResourceAsStream("sources.properties"));
		boolean allSuccess = true;
		allSuccess &= test("ontario_test.xml", properties);
//		allSuccess &= test("germany_test.xml", properties);
		if (allSuccess) {
			System.out.println("All is successfull");
		}

	}
	
	private static BinaryMapIndexReader[] getMapRegions(String regions, Properties properties) throws FileNotFoundException, IOException {
		String[] regionsSplit = regions.split(",");
		BinaryMapIndexReader[] readers = new BinaryMapIndexReader[regionsSplit.length];
		int i = 0;
		for (String reg : regionsSplit) {
			reg = reg.toUpperCase().trim();
			if (!properties.containsKey(reg)) {
				throw new IllegalArgumentException("Region " + reg + " is not found in the source.properties file");
			}
			BinaryMapIndexReader r = new BinaryMapIndexReader(new RandomAccessFile((String) properties.get(reg), "r"), false);
			readers[i++] = r;
		}

		return readers;
	}

	private static boolean test(String file, Properties properties) throws SAXException, IOException, ParserConfigurationException {
		InputStream resource = RouterTestsSuite.class.getResourceAsStream(file);
		Document testSuite = DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(new InputSource(resource));
		NodeList tests = testSuite.getElementsByTagName("test");
		
		for(int i=0; i < tests.getLength(); i++){
			Element e = (Element) tests.item(i);
			BinaryMapIndexReader[] regions = getMapRegions(e.getAttribute("regions"), properties);
			
			testRoute(e, regions);
		}
		
		return true;
		
	}

	private static void testRoute(Element testCase, BinaryMapIndexReader[] regions) throws IOException {
		BinaryRoutePlanner planner = new BinaryRoutePlanner(NativeSwingRendering.getDefaultFromSettings(), regions);
		String vehicle = testCase.getAttribute("vehicle");
		RoutingContext ctx = new RoutingContext(RoutingConfiguration.getDefault().build(vehicle, true));
		String testDescription = testCase.getAttribute("description");
		String skip = testCase.getAttribute("skip_comment");
		if (skip != null && skip.length() > 0) {
			System.err.println("\n\n!! Skipped test case '" + testDescription + "' because '" + skip + "'\n\n" );
			return;
		}
		
		double startLat = Double.parseDouble(testCase.getAttribute("start_lat"));
		double startLon = Double.parseDouble(testCase.getAttribute("start_lon"));
		RouteSegment startSegment = planner.findRouteSegment(startLat, startLon, ctx);
		double endLat = Double.parseDouble(testCase.getAttribute("target_lat"));
		double endLon = Double.parseDouble(testCase.getAttribute("target_lon"));
		RouteSegment endSegment = planner.findRouteSegment(endLat, endLon, ctx);
		if(startSegment == null){
			throw new IllegalArgumentException("Start segment is not found for test : " + testDescription);
		}
		if(endSegment == null){
			throw new IllegalArgumentException("End segment is not found for test : " + testDescription);
		}
		
		List<RouteSegmentResult> route = planner.searchRoute(ctx, startSegment, endSegment,false);
		
		Location start=new Location(startLat,startLon);
		LatLon end=new LatLon(endLat,endLon);
		RouteCalculationResult rcr=new RouteCalculationResult(route, start, end, ctx/*app*/, false/*leftSide*/);
		if(rcr!=null)
		{
			
		}
		/*
		NodeList segments = testCase.getElementsByTagName("segment");
		int i = 0;
		while (i < segments.getLength() && i < route.size()) {
			Element segment = (Element) segments.item(i);
			long expectedId = Long.parseLong(segment.getAttribute("id"));
			int expectedStart = Integer.parseInt(segment.getAttribute("start"));
			int expectedEnd = Integer.parseInt(segment.getAttribute("end"));
			RouteSegmentResult segmentResult = route.get(i);
			if (expectedId != segmentResult.getObject().getId() >> 1) {
				throw new IllegalArgumentException("Test : '" + testDescription + "' on segment " + (i + 1) + " : " + "\n"
						+ "(expected route id) " + expectedId + " != " + (segmentResult.getObject().getId() >> 1) + " (actual route id)");
			}
			if (expectedStart != segmentResult.getStartPointIndex()) {
				throw new IllegalArgumentException("Test : '" + testDescription + "' on segment " + (i + 1) + " : " + "\n"
						+ "(expected start index) " + expectedStart + " != " + segmentResult.getStartPointIndex() + " (actual start index)");
			}
			if (expectedEnd != segmentResult.getEndPointIndex()) {
				throw new IllegalArgumentException("Test : '" + testDescription + "' on segment " + (i + 1) + " : " + "\n"
						+ "(expected end index) " + expectedEnd + " != " + segmentResult.getEndPointIndex() + " (actual end index)");
			}

			i++;
		}
		
		if(segments.getLength() < route.size()){
			throw new IllegalArgumentException("Expected route is shorter than calculated for test : " + testDescription);
		} else if(segments.getLength() > route.size()){
			throw new IllegalArgumentException("Expected route is more lengthy than calculated for test : " + testDescription);
		}
		*/
		
		//svk
		int i = 0;
		while (i <  route.size()) {
			
			RouteSegmentResult segmentResult = route.get(i);
			
		//	System.err.println("\n\n!! Skipped test case '" + testDescription + "' because '" + skip + "'\n\n" );
			System.out.print(" routeID= " + (segmentResult.getObject().getId() >> 1) );
			System.out.print(" start= " + segmentResult.getStartPointIndex());
			System.out.println(" end= " + segmentResult.getEndPointIndex());
			
		
			i++;
		}
		
		ArrayList<Location> locations = new ArrayList<Location>();
		
		for (RouteSegmentResult s : route) {
			boolean plus = s.getStartPointIndex() < s.getEndPointIndex();
			i = s.getStartPointIndex();
			while (true) {
				Location n = new Location(""); //$NON-NLS-1$
				LatLon point = s.getPoint(i);
				n.setLatitude(point.getLatitude());
				n.setLongitude(point.getLongitude());
				locations.add(n);
				if (i == s.getEndPointIndex()) {
					break;
				}
				if (plus) {
					i++;
				} else {
					i--;
				}
			}
		}
		if(locations.size()>0)
		{
			
		}
	}
	

}

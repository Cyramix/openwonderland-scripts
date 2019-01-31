/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jdesktop.wonderland.modules.subsnapshots.client;

import java.io.StringWriter;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import org.jdesktop.wonderland.client.cell.Cell;
import org.jdesktop.wonderland.client.cell.asset.AssetUtils;
import org.jdesktop.wonderland.client.contextmenu.ContextMenuActionListener;
import org.jdesktop.wonderland.client.contextmenu.ContextMenuItem;
import org.jdesktop.wonderland.client.contextmenu.ContextMenuItemEvent;
import org.jdesktop.wonderland.client.contextmenu.SimpleContextMenuItem;
import org.jdesktop.wonderland.client.contextmenu.annotation.ContextMenuFactory;
import org.jdesktop.wonderland.client.contextmenu.spi.ContextMenuFactorySPI;
import org.jdesktop.wonderland.client.scenemanager.event.ContextEvent;
import org.jdesktop.wonderland.common.cell.messages.CellServerStateRequestMessage;
import org.jdesktop.wonderland.common.cell.messages.CellServerStateResponseMessage;
import org.jdesktop.wonderland.common.cell.state.CellServerState;
import org.jdesktop.wonderland.common.cell.state.CellServerStateFactory;
import org.jdesktop.wonderland.common.messages.ResponseMessage;
import org.jdesktop.wonderland.common.utils.ScannedClassLoader;

/**
 *
 * @author WonderlandWednesday
 */
@ContextMenuFactory
public class SubsnapshotContextMenuFactory implements ContextMenuFactorySPI {
    public ContextMenuItem[] getContextMenuItems(ContextEvent event) {
        return new ContextMenuItem[] {
            new SimpleContextMenuItem("Export", new ContextMenuActionListener()
            {
                public void actionPerformed(ContextMenuItemEvent event) {
                    Cell cell = event.getCell();
                    if(cell == null) {
                        return;
                    }
                    //TODO
                    //launch saveas common dialog and grab location JFileChooser,
                    // possibly in HUD.
                    exportCell(cell);
                }
            })
        };
    }

    public void exportCell(Cell cell) {

        ResponseMessage rm = cell.sendCellMessageAndWait(
                //CellServerState requst message here.
                new CellServerStateRequestMessage(cell.getCellID())
                );
        if(rm == null) {
        //    return null;
        }
        CellServerStateResponseMessage stateMessage = (CellServerStateResponseMessage)rm;
        CellServerState state = stateMessage.getCellServerState();
        StringWriter sWriter = new StringWriter();
        try {
            ScannedClassLoader loader =
                    cell.getCellCache().getSession().getSessionManager().getClassloader();

            state.encode(sWriter, CellServerStateFactory.getMarshaller(loader));
            System.out.println(sWriter.getBuffer());
            String s = sWriter.getBuffer().toString();
            
            List <String> uriList = new ArrayList();
            
            int uri0 = 0;
            int uri1 = 0;

            while(true) {
                uri0 = s.indexOf("wlcontent:", uri1);
                if (uri0 == -1) break;
                uri1 = s.indexOf(s, uri0);
                if (uri1 == -1) break;

                // we have 2 valid index values
                uriList.add(s.substring(uri0, uri1-1));

            }
            for(String uri : uriList) {
                URL url = AssetUtils.getAssetURL(uri, cell);
                //TODO
                //grab resource from server
                url.openStream();
            }

        } catch(Exception e) {
            e.printStackTrace();
        }
        //not complete
    }

}

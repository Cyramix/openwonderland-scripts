/**
 * Project Wonderland
 *
 * Copyright (c) 2004-2009, Sun Microsystems, Inc., All Rights Reserved
 *
 * Redistributions in source code form must reproduce the above
 * copyright and this condition.
 *
 * The contents of this file are subject to the GNU General Public
 * License, Version 2 (the "License"); you may not use this file
 * except in compliance with the License. A copy of the License is
 * available at http://www.opensource.org/licenses/gpl-license.php.
 *
 * Sun designates this particular file as subject to the "Classpath" 
 * exception as provided by Sun in the License file that accompanied 
 * this code.
 */
package org.jdesktop.wonderland.modules.statussample.server;

import org.jdesktop.wonderland.common.ExperimentalAPI;
import org.jdesktop.wonderland.common.cell.ClientCapabilities;
import org.jdesktop.wonderland.server.cell.CellMO;
import org.jdesktop.wonderland.common.cell.state.CellServerState;
import org.jdesktop.wonderland.modules.statussample.common.SampleCellServerState;
import org.jdesktop.wonderland.server.comms.WonderlandClientID;

/**
 * A sample cell
 * @author jkaplan
 */
@ExperimentalAPI
public class SampleCellMO extends CellMO { 
    /** Default constructor, used when cell is created via WFS */
    public SampleCellMO() {
    }

    @Override
    protected void setLive(boolean live) {
        super.setLive(live);
    }

    @Override 
    protected String getClientCellClassName(WonderlandClientID clientID, ClientCapabilities capabilities) {
        return "org.jdesktop.wonderland.modules.statussample.client.SampleCell";
    }

    @Override
    public CellServerState getServerState(CellServerState cellServerState) {
        if (cellServerState == null) {
            cellServerState = new SampleCellServerState();
        }
        return super.getServerState(cellServerState);
    }
}

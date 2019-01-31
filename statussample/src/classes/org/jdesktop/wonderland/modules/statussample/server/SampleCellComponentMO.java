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

import org.jdesktop.wonderland.common.cell.state.CellComponentServerState;
import org.jdesktop.wonderland.modules.statussample.common.SampleCellComponentServerState;
import org.jdesktop.wonderland.server.cell.CellComponentMO;
import org.jdesktop.wonderland.server.cell.CellMO;

/**
 * A sample cell component
 *
 * @author Jordan Slott <jslott@dev.java.net>
 */
public class SampleCellComponentMO extends CellComponentMO {
    public SampleCellComponentMO(CellMO cell) {
        super(cell);
    }

    @Override
    protected String getClientClass() {
        return "org.jdesktop.wonderland.modules.statussample.client.SampleCellComponent";
    }

    @Override
    protected void setLive(boolean live) {
        super.setLive(live);
    }

    @Override
    public CellComponentServerState getServerState(CellComponentServerState state) {
        if (state == null) {
            state = new SampleCellComponentServerState();
        }
        return super.getServerState(state);
    }
}

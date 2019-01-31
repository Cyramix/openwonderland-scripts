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
package org.jdesktop.wonderland.modules.statussample.client;

import java.util.logging.Logger;
import org.jdesktop.wonderland.client.cell.Cell;
import org.jdesktop.wonderland.client.cell.Cell.RendererType;
import org.jdesktop.wonderland.client.cell.CellCache;
import org.jdesktop.wonderland.client.cell.CellRenderer;
import org.jdesktop.wonderland.common.cell.CellID;
import org.jdesktop.wonderland.common.cell.CellStatus;

/**
 * Client-side cell for rendering JME content
 * 
 * @author jkaplan
 * @author Ronny Standtke <ronny.standtke@fhnw.ch>
 */
public class SampleCell extends Cell {
    private static Logger logger = 
            Logger.getLogger(SampleCell.class.getName());

    /* The type of shape: BOX or SPHERE */
    private SampleRenderer cellRenderer = null;

    public SampleCell(CellID cellID, CellCache cellCache) {
        super(cellID, cellCache);
    }

    @Override
    protected CellRenderer createCellRenderer(RendererType rendererType) {
        if (rendererType == RendererType.RENDERER_JME) {
            cellRenderer = new SampleRenderer(this);
            return cellRenderer;
        }
        return super.createCellRenderer(rendererType);
    }

    @Override
    protected void setStatus(CellStatus status, boolean increasing) {
        logger.warning("[Cell Status] " + status);
        super.setStatus(status, increasing);
    }
}

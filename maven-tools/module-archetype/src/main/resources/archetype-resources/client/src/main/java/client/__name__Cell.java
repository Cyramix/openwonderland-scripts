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
package ${package}.client;

import org.jdesktop.wonderland.client.cell.Cell;
import org.jdesktop.wonderland.client.cell.Cell.RendererType;
import org.jdesktop.wonderland.client.cell.CellCache;
import org.jdesktop.wonderland.client.cell.CellRenderer;
import org.jdesktop.wonderland.common.cell.CellID;
import org.jdesktop.wonderland.common.cell.state.CellClientState;
import ${package}.common.${name}ClientState;

/**
 * Client-side cell for rendering.
 */
public class ArchetypeCell extends Cell {
    //
    // cell state
    //

    private CellRenderer cellRenderer;

    //
    // any cell components the cell depends on
    //

    // @UsesCellComponent ContextMenuComponent menuComponent;

    /**
     * Default constructor taking cellID and CellCache is required.
     * @param cellID the id of the cell
     * @param cellCache the cell cache
     */
    public ArchetypeCell(CellID cellID, CellCache cellCache) {
        super(cellID, cellCache);
    }
    
    /**
     * Called when the cell is initially created and any time there is a 
     * major configuration change. The cell will already be attached to it
     * parent before the initial call of this method
     * 
     * @param clientState the client state to set
     */
    @Override
    public void setClientState(CellClientState cs) {
        super.setClientState(cs);
        
        // set the cell-specific values
        ArchetypeClientState state = (ArchetypeClientState) cs;
    }

    /**
     * Create the cell renderer that will be used to draw this cell
     * @param rendererType the type of rendering (JME right now)
     * @return the renderer for this cell
     */
    @Override
    protected CellRenderer createCellRenderer(RendererType rendererType) {
        if (rendererType == RendererType.RENDERER_JME) {
            cellRenderer = new ArchetypeCellRenderer(this);
        } else {
            // we don't know how to handle this render type
            cellRenderer = super.createCellRenderer(rendererType);
        }

        return cellRenderer;
    }

    /**
     * Called when the cell's status changes. Use this method to register
     * event listeners, add menu items, etc.
     * @param status the cell status
     * @return true if the status changed, or false if not
     */
    // @Override
    // public boolean setStatus(CellStatus status) {
    //    boolean ret = super.setStatus(status);
    //    switch (status) {
    //        case DISK:
                // clean up
    //            break;
    //        case INACTIVE:
                // add listeners and so on
    //            break;
    //    }
    //    return ret;
    //}
}
